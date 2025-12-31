/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   kraken-desktop-link-monitor.c: singleton that manages the links

   Copyright (C) 2003 Red Hat, Inc.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public
   License along with this program; if not, write to the
   Free Software Foundation, Inc., 51 Franklin Street - Suite 500,
   Boston, MA 02110-1335, USA.

   Author: Alexander Larsson <alexl@redhat.com>
*/

#include <config.h>
#include "kraken-desktop-link-monitor.h"
#include "kraken-desktop-metadata.h"
#include "kraken-desktop-link.h"
#include "kraken-desktop-icon-file.h"
#include "kraken-directory.h"
#include "kraken-desktop-directory.h"
#include "kraken-global-preferences.h"

#include <eel/eel-debug.h>
#include <eel/eel-vfs-extensions.h>
#include <eel/eel-stock-dialogs.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <gio/gio.h>
#include <libkraken-private/kraken-trash-monitor.h>
#include <string.h>

struct KrakenDesktopLinkMonitorDetails {
	GVolumeMonitor *volume_monitor;
	KrakenDirectory *desktop_dir;

	KrakenDesktopLink *home_link;
	KrakenDesktopLink *computer_link;
	KrakenDesktopLink *trash_link;
	KrakenDesktopLink *network_link;

	GList *mount_links;
};

G_DEFINE_TYPE (KrakenDesktopLinkMonitor, kraken_desktop_link_monitor, G_TYPE_OBJECT);

static KrakenDesktopLinkMonitor *the_link_monitor = NULL;

static void
destroy_desktop_link_monitor (void)
{
	if (the_link_monitor != NULL) {
		g_object_unref (the_link_monitor);
	}
}

KrakenDesktopLinkMonitor *
kraken_desktop_link_monitor_get (void)
{
	if (the_link_monitor == NULL) {
		g_object_new (KRAKEN_TYPE_DESKTOP_LINK_MONITOR, NULL);
		eel_debug_call_at_shutdown (destroy_desktop_link_monitor);
	}
	return the_link_monitor;
}

static void
volume_delete_dialog (GtkWidget *parent_view,
                      KrakenDesktopLink *link)
{
	GMount *mount;
	char *dialog_str;
	char *display_name;

	mount = kraken_desktop_link_get_mount (link);

	if (mount != NULL) {
		display_name = kraken_desktop_link_get_display_name (link);
		dialog_str = g_strdup_printf (_("You cannot move the volume \"%s\" to the trash."),
					      display_name);
		g_free (display_name);

		if (g_mount_can_eject (mount)) {
			eel_run_simple_dialog
				(parent_view,
				 FALSE,
				 GTK_MESSAGE_ERROR,
				 dialog_str,
				 _("If you want to eject the volume, please use \"Eject\" in the "
				   "popup menu of the volume."),
				 GTK_STOCK_OK, NULL);
		} else {
			eel_run_simple_dialog
				(parent_view,
				 FALSE,
				 GTK_MESSAGE_ERROR,
				 dialog_str,
				 _("If you want to unmount the volume, please use \"Unmount Volume\" in the "
				   "popup menu of the volume."),
				 GTK_STOCK_OK, NULL);
		}

		g_object_unref (mount);
		g_free (dialog_str);
	}
}

void
kraken_desktop_link_monitor_delete_link (KrakenDesktopLinkMonitor *monitor,
					   KrakenDesktopLink *link,
					   GtkWidget *parent_view)
{
	switch (kraken_desktop_link_get_link_type (link)) {
	case KRAKEN_DESKTOP_LINK_HOME:
	case KRAKEN_DESKTOP_LINK_COMPUTER:
	case KRAKEN_DESKTOP_LINK_TRASH:
	case KRAKEN_DESKTOP_LINK_NETWORK:
		/* just ignore. We don't allow you to delete these */
		break;
        case KRAKEN_DESKTOP_LINK_MOUNT:
	default:
		volume_delete_dialog (parent_view, link);
		break;
	}
}

static gboolean
volume_file_name_used (KrakenDesktopLinkMonitor *monitor,
		       const char *name)
{
	GList *l;
	char *other_name;
	gboolean same;

	for (l = monitor->details->mount_links; l != NULL; l = l->next) {
		other_name = kraken_desktop_link_get_file_name (l->data);
		same = strcmp (name, other_name) == 0;
		g_free (other_name);

		if (same) {
			return TRUE;
		}
	}

	return FALSE;
}

char *
kraken_desktop_link_monitor_make_filename_unique (KrakenDesktopLinkMonitor *monitor,
						    const char *filename)
{
	char *unique_name;
	int i;

	i = 2;
	unique_name = g_strdup (filename);
	while (volume_file_name_used (monitor, unique_name)) {
		g_free (unique_name);
		unique_name = g_strdup_printf ("%s.%d", filename, i++);
	}
	return unique_name;
}

static gboolean
has_mount (KrakenDesktopLinkMonitor *monitor,
	   GMount                     *mount)
{
	gboolean ret;
	GMount *other_mount;
	GList *l;

	ret = FALSE;

	for (l = monitor->details->mount_links; l != NULL; l = l->next) {
		other_mount = kraken_desktop_link_get_mount (l->data);
		if (mount == other_mount) {
			g_object_unref (other_mount);
			ret = TRUE;
			break;
		}
		g_object_unref (other_mount);
	}

	return ret;
}

static void
create_mount_link (KrakenDesktopLinkMonitor *monitor,
		   GMount *mount)
{
	KrakenDesktopLink *link;

	if (has_mount (monitor, mount))
		return;

	if ((!g_mount_is_shadowed (mount)) &&
	    g_settings_get_boolean (kraken_desktop_preferences,
				    KRAKEN_PREFERENCES_DESKTOP_VOLUMES_VISIBLE)) {
		link = kraken_desktop_link_new_from_mount (mount);
		monitor->details->mount_links = g_list_prepend (monitor->details->mount_links, link);
	}
}

static void
remove_mount_link (KrakenDesktopLinkMonitor *monitor,
		   GMount *mount)
{
	GList *l;
	KrakenDesktopLink *link;
	GMount *other_mount;

	link = NULL;
	for (l = monitor->details->mount_links; l != NULL; l = l->next) {
		other_mount = kraken_desktop_link_get_mount (l->data);
		if (mount == other_mount) {
			g_object_unref (other_mount);
			link = l->data;
			break;
		}
		g_object_unref (other_mount);
	}

	if (link) {
		monitor->details->mount_links = g_list_remove (monitor->details->mount_links, link);
		g_object_unref (link);
	}
}



static void
mount_added_callback (GVolumeMonitor *volume_monitor,
		      GMount *mount,
		      KrakenDesktopLinkMonitor *monitor)
{
	create_mount_link (monitor, mount);
}


static void
mount_removed_callback (GVolumeMonitor *volume_monitor,
			GMount *mount,
			KrakenDesktopLinkMonitor *monitor)
{
	remove_mount_link (monitor, mount);
}

static void
mount_changed_callback (GVolumeMonitor *volume_monitor,
			GMount *mount,
			KrakenDesktopLinkMonitor *monitor)
{
	/* TODO: update the mount with other details */

	/* remove a mount if it goes into the shadows */
	if (g_mount_is_shadowed (mount) && has_mount (monitor, mount)) {
		remove_mount_link (monitor, mount);
	}}

static void
update_link_visibility (KrakenDesktopLinkMonitor *monitor,
			KrakenDesktopLink       **link_ref,
			KrakenDesktopLinkType     link_type,
			const char                 *preference_key)
{
	if (g_settings_get_boolean (kraken_desktop_preferences, preference_key)) {
		if (*link_ref == NULL) {
			*link_ref = kraken_desktop_link_new (link_type);
		}
	} else {
		if (*link_ref != NULL) {
            /* If this were a real file, removing (deleting or moving) it would
             * also remove its metadata, though for a different reason, and
             * unmanaged by us.  We have to simulate that when removing a fake
             * 'desktop' file, so if it gets added again later, it behaves like a
             * 'new' file.
             */
            kraken_desktop_clear_metadata (kraken_desktop_link_get_file (*link_ref));

			g_object_unref (*link_ref);
			*link_ref = NULL;
		}
	}
}

static void
desktop_home_visible_changed (gpointer callback_data)
{
	KrakenDesktopLinkMonitor *monitor;

	monitor = KRAKEN_DESKTOP_LINK_MONITOR (callback_data);

	update_link_visibility (KRAKEN_DESKTOP_LINK_MONITOR (monitor),
				&monitor->details->home_link,
				KRAKEN_DESKTOP_LINK_HOME,
				KRAKEN_PREFERENCES_DESKTOP_HOME_VISIBLE);
}

static void
desktop_computer_visible_changed (gpointer callback_data)
{
	KrakenDesktopLinkMonitor *monitor;

	monitor = KRAKEN_DESKTOP_LINK_MONITOR (callback_data);

	update_link_visibility (KRAKEN_DESKTOP_LINK_MONITOR (callback_data),
				&monitor->details->computer_link,
				KRAKEN_DESKTOP_LINK_COMPUTER,
				KRAKEN_PREFERENCES_DESKTOP_COMPUTER_VISIBLE);
}

static void
desktop_trash_visible_changed (gpointer callback_data)
{
	KrakenDesktopLinkMonitor *monitor;

	monitor = KRAKEN_DESKTOP_LINK_MONITOR (callback_data);

	update_link_visibility (KRAKEN_DESKTOP_LINK_MONITOR (callback_data),
				&monitor->details->trash_link,
				KRAKEN_DESKTOP_LINK_TRASH,
				KRAKEN_PREFERENCES_DESKTOP_TRASH_VISIBLE);
}

static void
desktop_network_visible_changed (gpointer callback_data)
{
	KrakenDesktopLinkMonitor *monitor;

	monitor = KRAKEN_DESKTOP_LINK_MONITOR (callback_data);

	update_link_visibility (KRAKEN_DESKTOP_LINK_MONITOR (callback_data),
				&monitor->details->network_link,
				KRAKEN_DESKTOP_LINK_NETWORK,
				KRAKEN_PREFERENCES_DESKTOP_NETWORK_VISIBLE);
}

static void
desktop_volumes_visible_changed (gpointer callback_data)
{
	KrakenDesktopLinkMonitor *monitor;
	GList *l, *mounts;

	monitor = KRAKEN_DESKTOP_LINK_MONITOR (callback_data);

	if (g_settings_get_boolean (kraken_desktop_preferences,
				    KRAKEN_PREFERENCES_DESKTOP_VOLUMES_VISIBLE)) {
		if (monitor->details->mount_links == NULL) {
			mounts = g_volume_monitor_get_mounts (monitor->details->volume_monitor);
			for (l = mounts; l != NULL; l = l->next) {
				create_mount_link (monitor, l->data);
				g_object_unref (l->data);
			}
			g_list_free (mounts);
		}
	} else {
		g_list_foreach (monitor->details->mount_links, (GFunc)g_object_unref, NULL);
		g_list_free (monitor->details->mount_links);
		monitor->details->mount_links = NULL;
	}
}

static void
create_link_and_add_preference (KrakenDesktopLink   **link_ref,
				KrakenDesktopLinkType link_type,
				const char             *preference_key,
				GCallback               callback,
				gpointer                callback_data)
{
	char *detailed_signal;

	if (g_settings_get_boolean (kraken_desktop_preferences, preference_key)) {
		*link_ref = kraken_desktop_link_new (link_type);
	}

	detailed_signal = g_strconcat ("changed::", preference_key, NULL);
	g_signal_connect_swapped (kraken_desktop_preferences,
				  detailed_signal,
				  callback, callback_data);

	g_free (detailed_signal);
}

static void
kraken_desktop_link_monitor_init (KrakenDesktopLinkMonitor *monitor)
{
	GList *l, *mounts;
	GMount *mount;

	monitor->details = G_TYPE_INSTANCE_GET_PRIVATE (monitor, KRAKEN_TYPE_DESKTOP_LINK_MONITOR,
							KrakenDesktopLinkMonitorDetails);

	the_link_monitor = monitor;
	monitor->details->volume_monitor = g_volume_monitor_get ();

	/* We keep around a ref to the desktop dir */
	monitor->details->desktop_dir = kraken_directory_get_by_uri (EEL_DESKTOP_URI);

	/* Default links */

	create_link_and_add_preference (&monitor->details->home_link,
					KRAKEN_DESKTOP_LINK_HOME,
					KRAKEN_PREFERENCES_DESKTOP_HOME_VISIBLE,
					G_CALLBACK (desktop_home_visible_changed),
					monitor);

	create_link_and_add_preference (&monitor->details->computer_link,
					KRAKEN_DESKTOP_LINK_COMPUTER,
					KRAKEN_PREFERENCES_DESKTOP_COMPUTER_VISIBLE,
					G_CALLBACK (desktop_computer_visible_changed),
					monitor);

	create_link_and_add_preference (&monitor->details->trash_link,
					KRAKEN_DESKTOP_LINK_TRASH,
					KRAKEN_PREFERENCES_DESKTOP_TRASH_VISIBLE,
					G_CALLBACK (desktop_trash_visible_changed),
					monitor);

	create_link_and_add_preference (&monitor->details->network_link,
					KRAKEN_DESKTOP_LINK_NETWORK,
					KRAKEN_PREFERENCES_DESKTOP_NETWORK_VISIBLE,
					G_CALLBACK (desktop_network_visible_changed),
					monitor);

	/* Mount links */

	mounts = g_volume_monitor_get_mounts (monitor->details->volume_monitor);
	for (l = mounts; l != NULL; l = l->next) {
		mount = l->data;
		create_mount_link (monitor, mount);
		g_object_unref (mount);
	}
	g_list_free (mounts);

	g_signal_connect_swapped (kraken_desktop_preferences,
				  "changed::" KRAKEN_PREFERENCES_DESKTOP_VOLUMES_VISIBLE,
				  G_CALLBACK (desktop_volumes_visible_changed),
				  monitor);

	g_signal_connect_object (monitor->details->volume_monitor, "mount_added",
				 G_CALLBACK (mount_added_callback), monitor, 0);
	g_signal_connect_object (monitor->details->volume_monitor, "mount_removed",
				 G_CALLBACK (mount_removed_callback), monitor, 0);
	g_signal_connect_object (monitor->details->volume_monitor, "mount_changed",
				 G_CALLBACK (mount_changed_callback), monitor, 0);

}

static void
remove_link_and_preference (KrakenDesktopLink   **link_ref,
			    const char             *preference_key,
			    GCallback               callback,
			    gpointer                callback_data)
{
	if (*link_ref != NULL) {
		g_object_unref (*link_ref);
		*link_ref = NULL;
	}

	g_signal_handlers_disconnect_by_func (kraken_desktop_preferences,
					      callback, callback_data);
}

static void
desktop_link_monitor_finalize (GObject *object)
{
	KrakenDesktopLinkMonitor *monitor;

	monitor = KRAKEN_DESKTOP_LINK_MONITOR (object);

	g_object_unref (monitor->details->volume_monitor);

	/* Default links */

	remove_link_and_preference (&monitor->details->home_link,
				    KRAKEN_PREFERENCES_DESKTOP_HOME_VISIBLE,
				    G_CALLBACK (desktop_home_visible_changed),
				    monitor);

	remove_link_and_preference (&monitor->details->computer_link,
				    KRAKEN_PREFERENCES_DESKTOP_COMPUTER_VISIBLE,
				    G_CALLBACK (desktop_computer_visible_changed),
				    monitor);

	remove_link_and_preference (&monitor->details->trash_link,
				    KRAKEN_PREFERENCES_DESKTOP_TRASH_VISIBLE,
				    G_CALLBACK (desktop_trash_visible_changed),
				    monitor);

	remove_link_and_preference (&monitor->details->network_link,
				    KRAKEN_PREFERENCES_DESKTOP_NETWORK_VISIBLE,
				    G_CALLBACK (desktop_network_visible_changed),
				    monitor);

	/* Mounts */

	g_list_foreach (monitor->details->mount_links, (GFunc)g_object_unref, NULL);
	g_list_free (monitor->details->mount_links);
	monitor->details->mount_links = NULL;

	kraken_directory_unref (monitor->details->desktop_dir);
	monitor->details->desktop_dir = NULL;

	g_signal_handlers_disconnect_by_func (kraken_desktop_preferences,
					      desktop_volumes_visible_changed,
					      monitor);

	G_OBJECT_CLASS (kraken_desktop_link_monitor_parent_class)->finalize (object);
}

static void
kraken_desktop_link_monitor_class_init (KrakenDesktopLinkMonitorClass *klass)
{
	GObjectClass *object_class;

	object_class = G_OBJECT_CLASS (klass);
	object_class->finalize = desktop_link_monitor_finalize;

	g_type_class_add_private (klass, sizeof (KrakenDesktopLinkMonitorDetails));
}
