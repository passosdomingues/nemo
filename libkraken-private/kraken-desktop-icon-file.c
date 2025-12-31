/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   kraken-desktop-icon-file.c: Subclass of KrakenFile to help implement the
   virtual desktop icons.
 
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
#include "kraken-desktop-icon-file.h"

#include "kraken-desktop-metadata.h"
#include "kraken-desktop-directory-file.h"
#include "kraken-directory-notify.h"
#include "kraken-directory-private.h"
#include "kraken-file-attributes.h"
#include "kraken-file-private.h"
#include "kraken-file-utilities.h"
#include "kraken-file-operations.h"
#include <eel/eel-glib-extensions.h>
#include "kraken-desktop-directory.h"
#include <glib/gi18n.h>
#include <string.h>
#include <gio/gio.h>

struct KrakenDesktopIconFileDetails {
	KrakenDesktopLink *link;
};

G_DEFINE_TYPE(KrakenDesktopIconFile, kraken_desktop_icon_file, KRAKEN_TYPE_FILE)


static void
desktop_icon_file_monitor_add (KrakenFile *file,
			       gconstpointer client,
			       KrakenFileAttributes attributes)
{
	kraken_directory_monitor_add_internal
		(file->details->directory, file,
		 client, TRUE, attributes, NULL, NULL);
}

static void
desktop_icon_file_monitor_remove (KrakenFile *file,
				  gconstpointer client)
{
	kraken_directory_monitor_remove_internal
		(file->details->directory, file, client);
}

static void
desktop_icon_file_call_when_ready (KrakenFile *file,
				   KrakenFileAttributes attributes,
				   KrakenFileCallback callback,
				   gpointer callback_data)
{
	kraken_directory_call_when_ready_internal
		(file->details->directory, file,
		 attributes, FALSE, NULL, callback, callback_data);
}

static void
desktop_icon_file_cancel_call_when_ready (KrakenFile *file,
					  KrakenFileCallback callback,
					  gpointer callback_data)
{
	kraken_directory_cancel_callback_internal
		(file->details->directory, file,
		 NULL, callback, callback_data);
}

static gboolean
desktop_icon_file_check_if_ready (KrakenFile *file,
				  KrakenFileAttributes attributes)
{
	return kraken_directory_check_if_ready_internal
		(file->details->directory, file,
		 attributes);
}

static gboolean
desktop_icon_file_get_item_count (KrakenFile *file, 
				  guint *count,
				  gboolean *count_unreadable)
{
	if (count != NULL) {
		*count = 0;
	}
	if (count_unreadable != NULL) {
		*count_unreadable = FALSE;
	}
	return TRUE;
}

static KrakenRequestStatus
desktop_icon_file_get_deep_counts (KrakenFile *file,
				   guint *directory_count,
				   guint *file_count,
				   guint *unreadable_directory_count,
                   guint *hidden_count,
				   goffset *total_size)
{
	if (directory_count != NULL) {
		*directory_count = 0;
	}
	if (file_count != NULL) {
		*file_count = 0;
	}
	if (unreadable_directory_count != NULL) {
		*unreadable_directory_count = 0;
	}
	if (total_size != NULL) {
		*total_size = 0;
	}
    if (hidden_count != NULL) {
        *hidden_count = 0;
    }
	return KRAKEN_REQUEST_DONE;
}

static gboolean
desktop_icon_file_get_date (KrakenFile *file,
			    KrakenDateType date_type,
			    time_t *date)
{
	KrakenDesktopIconFile *desktop_file;

	desktop_file = KRAKEN_DESKTOP_ICON_FILE (file);

	return kraken_desktop_link_get_date (desktop_file->details->link,
					       date_type, date);
}

static char *
desktop_icon_file_get_where_string (KrakenFile *file)
{
	return g_strdup (_("on the desktop"));
}

static void
kraken_desktop_icon_file_init (KrakenDesktopIconFile *desktop_file)
{
	desktop_file->details =	G_TYPE_INSTANCE_GET_PRIVATE (desktop_file,
							     KRAKEN_TYPE_DESKTOP_ICON_FILE,
							     KrakenDesktopIconFileDetails);
}

static void
update_info_from_link (KrakenDesktopIconFile *icon_file)
{
	KrakenFile *file;
	KrakenDesktopLink *link;
	char *display_name;
	GMount *mount;
	
	file = KRAKEN_FILE (icon_file);
	
	link = icon_file->details->link;

	if (link == NULL) {
		return;
	}

	g_clear_pointer (&file->details->mime_type, g_ref_string_release);
	file->details->mime_type = g_ref_string_new_intern ("application/x-kraken-link");
	file->details->type = G_FILE_TYPE_SHORTCUT;
	file->details->size = 0;
	file->details->has_permissions = FALSE;
	file->details->can_read = TRUE;
	file->details->can_write = TRUE;

	file->details->can_mount = FALSE;
	file->details->can_unmount = FALSE;
	file->details->can_eject = FALSE;
	if (file->details->mount) {
		g_object_unref (file->details->mount);
	}
	mount = kraken_desktop_link_get_mount (link);
	file->details->mount = mount;
	if (mount) {
		file->details->can_unmount = g_mount_can_unmount (mount);
		file->details->can_eject = g_mount_can_eject (mount);
	}
	
	file->details->file_info_is_up_to_date = TRUE;

	display_name = kraken_desktop_link_get_display_name (link);
	kraken_file_set_display_name (file,
					display_name, NULL, TRUE);
	g_free (display_name);

	if (file->details->icon != NULL) {
		g_object_unref (file->details->icon);
	}
	file->details->icon = kraken_desktop_link_get_icon (link);
	g_free (file->details->activation_uri);
	file->details->activation_uri = kraken_desktop_link_get_activation_uri (link);
	file->details->got_link_info = TRUE;
	file->details->link_info_is_up_to_date = TRUE;

	file->details->directory_count = 0;
	file->details->got_directory_count = TRUE;
	file->details->directory_count_is_up_to_date = TRUE;
}

void
kraken_desktop_icon_file_update (KrakenDesktopIconFile *icon_file)
{
	KrakenFile *file;
	
	update_info_from_link (icon_file);
	file = KRAKEN_FILE (icon_file);
	kraken_file_changed (file);
}

void
kraken_desktop_icon_file_remove (KrakenDesktopIconFile *icon_file)
{
	KrakenFile *file;
	GList list;

	icon_file->details->link = NULL;

	file = KRAKEN_FILE (icon_file);
	
	/* ref here because we might be removing the last ref when we
	 * mark the file gone below, but we need to keep a ref at
	 * least long enough to send the change notification. 
	 */
	kraken_file_ref (file);
	
	file->details->is_gone = TRUE;
	
	list.data = file;
	list.next = NULL;
	list.prev = NULL;
	
	kraken_directory_remove_file (file->details->directory, file);
	kraken_directory_emit_change_signals (file->details->directory, &list);
	
	kraken_file_unref (file);
}

KrakenDesktopIconFile *
kraken_desktop_icon_file_new (KrakenDesktopLink *link)
{
	KrakenFile *file;
	KrakenDirectory *directory;
	KrakenDesktopIconFile *icon_file;
	GList list;
	char *name;

	directory = kraken_directory_get_by_uri (EEL_DESKTOP_URI);

	file = KRAKEN_FILE (g_object_new (KRAKEN_TYPE_DESKTOP_ICON_FILE, NULL));

#ifdef KRAKEN_FILE_DEBUG_REF
	printf("%10p ref'd\n", file);
	eazel_dump_stack_trace ("\t", 10);
#endif

	file->details->directory = directory;

	icon_file = KRAKEN_DESKTOP_ICON_FILE (file);
	icon_file->details->link = link;

	name = kraken_desktop_link_get_file_name (link);
	file->details->name = g_ref_string_new (name);
	g_free (name);

	update_info_from_link (icon_file);

	kraken_desktop_update_metadata_from_keyfile (file, file->details->name);

	kraken_directory_add_file (directory, file);

	list.data = file;
	list.next = NULL;
	list.prev = NULL;
	kraken_directory_emit_files_added (directory, &list);

	return icon_file;
}

/* Note: This can return NULL if the link was recently removed (i.e. unmounted) */
KrakenDesktopLink *
kraken_desktop_icon_file_get_link (KrakenDesktopIconFile *icon_file)
{
	if (icon_file->details->link)
		return g_object_ref (icon_file->details->link);
	else
		return NULL;
}

static void
kraken_desktop_icon_file_unmount (KrakenFile                   *file,
				    GMountOperation                *mount_op,
				    GCancellable                   *cancellable,
				    KrakenFileOperationCallback   callback,
				    gpointer                        callback_data)
{
	KrakenDesktopIconFile *desktop_file;
	GMount *mount;
	
	desktop_file = KRAKEN_DESKTOP_ICON_FILE (file);
	if (desktop_file) {
		mount = kraken_desktop_link_get_mount (desktop_file->details->link);
		if (mount != NULL) {
			kraken_file_operations_unmount_mount (NULL, mount, FALSE, TRUE);
		}
	}
	
}

static void
kraken_desktop_icon_file_eject (KrakenFile                   *file,
				  GMountOperation                *mount_op,
				  GCancellable                   *cancellable,
				  KrakenFileOperationCallback   callback,
				  gpointer                        callback_data)
{
	KrakenDesktopIconFile *desktop_file;
	GMount *mount;
	
	desktop_file = KRAKEN_DESKTOP_ICON_FILE (file);
	if (desktop_file) {
		mount = kraken_desktop_link_get_mount (desktop_file->details->link);
		if (mount != NULL) {
			kraken_file_operations_unmount_mount (NULL, mount, TRUE, TRUE);
		}
	}
}

static void
kraken_desktop_icon_file_set_metadata (KrakenFile           *file,
					 const char             *key,
					 const char             *value)
{
	kraken_desktop_set_metadata_string (file, file->details->name, key, value);
}

static void
kraken_desktop_icon_file_set_metadata_as_list (KrakenFile           *file,
						 const char             *key,
						 char                  **value)
{
	kraken_desktop_set_metadata_stringv (file, file->details->name, key, (const gchar **) value);
}

static void
kraken_desktop_icon_file_class_init (KrakenDesktopIconFileClass *klass)
{
	GObjectClass *object_class;
	KrakenFileClass *file_class;

	object_class = G_OBJECT_CLASS (klass);
	file_class = KRAKEN_FILE_CLASS (klass);

	file_class->default_file_type = G_FILE_TYPE_DIRECTORY;
	
	file_class->monitor_add = desktop_icon_file_monitor_add;
	file_class->monitor_remove = desktop_icon_file_monitor_remove;
	file_class->call_when_ready = desktop_icon_file_call_when_ready;
	file_class->cancel_call_when_ready = desktop_icon_file_cancel_call_when_ready;
	file_class->check_if_ready = desktop_icon_file_check_if_ready;
	file_class->get_item_count = desktop_icon_file_get_item_count;
	file_class->get_deep_counts = desktop_icon_file_get_deep_counts;
	file_class->get_date = desktop_icon_file_get_date;
	file_class->get_where_string = desktop_icon_file_get_where_string;
	file_class->set_metadata = kraken_desktop_icon_file_set_metadata;
	file_class->set_metadata_as_list = kraken_desktop_icon_file_set_metadata_as_list;
	file_class->unmount = kraken_desktop_icon_file_unmount;
	file_class->eject = kraken_desktop_icon_file_eject;

	g_type_class_add_private (object_class, sizeof(KrakenDesktopIconFileDetails));
}
