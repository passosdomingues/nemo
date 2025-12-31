/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* fm-desktop-icon-view.c - implementation of icon view for managing the desktop.

   Copyright (C) 2000, 2001 Eazel, Inc.mou

   The Gnome Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The Gnome Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 51 Franklin Street - Suite 500,
   Boston, MA 02110-1335, USA.

   Authors: Mike Engber <engber@eazel.com>
   	    Gene Z. Ragan <gzr@eazel.com>
	    Miguel de Icaza <miguel@ximian.com>
*/

#include <config.h>

#include "kraken-desktop-icon-view.h"

#include "kraken-actions.h"
#include "kraken-application.h"
#include "kraken-desktop-manager.h"
#include "kraken-desktop-window.h"
#include "kraken-icon-view-container.h"
#include "kraken-view-factory.h"
#include "kraken-view.h"

#include <X11/Xatom.h>
#include <gtk/gtk.h>
#include <eel/eel-glib-extensions.h>
#include <eel/eel-gtk-extensions.h>
#include <eel/eel-vfs-extensions.h>
#include <fcntl.h>
#include <gdk/gdkx.h>
#include <glib/gi18n.h>
#include <libkraken-private/kraken-desktop-icon-file.h>
#include <libkraken-private/kraken-directory-notify.h>
#include <libkraken-private/kraken-file-changes-queue.h>
#include <libkraken-private/kraken-file-operations.h>
#include <libkraken-private/kraken-file-utilities.h>
#include <libkraken-private/kraken-ui-utilities.h>
#include <libkraken-private/kraken-global-preferences.h>
#include <libkraken-private/kraken-link.h>
#include <libkraken-private/kraken-metadata.h>
#include <libkraken-private/kraken-monitor.h>
#include <libkraken-private/kraken-program-choosing.h>
#include <libkraken-private/kraken-trash-monitor.h>
#include <libkraken-private/kraken-desktop-utils.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/* Timeout to check the desktop directory for updates */
#define RESCAN_TIMEOUT 4

struct KrakenDesktopIconViewDetails
{
	GdkWindow *root_window;
	GtkActionGroup *desktop_action_group;
	guint desktop_merge_id;

	/* For the desktop rescanning
	 */
	gulong delayed_init_signal;
	guint reload_desktop_timeout;
	gboolean pending_rescan;
};

static void     default_zoom_level_changed                        (gpointer                user_data);
static void     real_merge_menus                                  (KrakenView        *view);
static void     real_update_menus                                 (KrakenView        *view);
static void     kraken_desktop_icon_view_update_icon_container_fonts  (KrakenDesktopIconView      *view);
static void     font_changed_callback                             (gpointer                callback_data);
static void     kraken_desktop_icon_view_constructed (GObject *object);

G_DEFINE_TYPE (KrakenDesktopIconView, kraken_desktop_icon_view, KRAKEN_TYPE_ICON_VIEW)

static char *desktop_directory;
static time_t desktop_dir_modify_time;

#define get_icon_container(w) kraken_icon_view_get_icon_container(KRAKEN_ICON_VIEW (w))

static void
desktop_directory_changed_callback (gpointer callback_data)
{
	g_free (desktop_directory);
	desktop_directory = kraken_get_desktop_directory ();
}

static void
update_margins (KrakenDesktopIconView *icon_view)
{
    KrakenIconContainer *icon_container;
    gint current_monitor;
    gint l, r, t, b;

    icon_container = get_icon_container (icon_view);

    g_object_get (KRAKEN_DESKTOP_WINDOW (gtk_widget_get_toplevel (GTK_WIDGET (icon_view))),
                  "monitor", &current_monitor,
                  NULL);

    kraken_desktop_manager_get_margins (kraken_desktop_manager_get (), current_monitor, &l, &r, &t, &b);

    kraken_icon_container_set_margins (icon_container, l, r, t, b);
}

static GdkFilterReturn
gdk_filter_func (GdkXEvent *gdk_xevent,
                 GdkEvent  *event,
                 gpointer   data)
{
    XEvent *xevent = gdk_xevent;
    KrakenDesktopIconView *icon_view;

    icon_view = KRAKEN_DESKTOP_ICON_VIEW (data);

    switch (xevent->type) {
        case PropertyNotify:
            if (xevent->xproperty.atom == gdk_x11_get_xatom_by_name ("_NET_WORKAREA")) {
                update_margins (icon_view);
            }
            break;
        default:
            break;
    }

    return GDK_FILTER_CONTINUE;
}

static const char *
real_get_id (KrakenView *view)
{
	return KRAKEN_DESKTOP_ICON_VIEW_ID;
}

static gboolean
should_show_file_on_current_monitor (KrakenView *view, KrakenFile *file)
{
    gint current_monitor = kraken_desktop_utils_get_monitor_for_widget (GTK_WIDGET (view));
    gint file_monitor = kraken_file_get_monitor_number (file);

    KrakenDesktopManager *dm = kraken_desktop_manager_get ();

    if (current_monitor == file_monitor) {
        return TRUE;
    }

    if (kraken_desktop_manager_get_primary_only (dm)) {
        return TRUE;
    }

    if (file_monitor > -1 &&
        !g_settings_get_boolean (kraken_desktop_preferences, KRAKEN_PREFERENCES_SHOW_ORPHANED_DESKTOP_ICONS)) {
        return FALSE;
    }

    if (file_monitor == -1) {
        /* New file, no previous metadata - this should go on the primary monitor */
        return kraken_desktop_manager_get_monitor_is_primary (dm, current_monitor);
    }

    if (!kraken_desktop_manager_get_monitor_is_active (dm, file_monitor)) {
        kraken_file_set_is_desktop_orphan (file, TRUE);
        if (kraken_desktop_manager_get_monitor_is_primary (dm, current_monitor)) {
            return TRUE;
        }
    }

    return FALSE;
}

static void
real_add_file (KrakenView *view, KrakenFile *file, KrakenDirectory *directory)
{
    KrakenIconView *icon_view;
    KrakenIconContainer *icon_container;

    g_assert (directory == kraken_view_get_model (view));

    if (!should_show_file_on_current_monitor (view, file)) {
        return;
    }

    icon_view = KRAKEN_ICON_VIEW (view);
    icon_container = get_icon_container (icon_view);

    if (kraken_icon_container_add (icon_container, KRAKEN_ICON_CONTAINER_ICON_DATA (file))) {
        kraken_file_ref (file);
    }
}

static void
unrealized_callback (GtkWidget *widget, KrakenDesktopIconView *desktop_icon_view)
{
  g_return_if_fail (desktop_icon_view->details->root_window != NULL);

  gdk_window_remove_filter (desktop_icon_view->details->root_window,
                            gdk_filter_func,
                            desktop_icon_view);

  desktop_icon_view->details->root_window = NULL;
}

static void
realized_callback (GtkWidget *widget, KrakenDesktopIconView *desktop_icon_view)
{
  GdkWindow *root_window;
  GdkScreen *screen;

  g_return_if_fail (desktop_icon_view->details->root_window == NULL);

  screen = gtk_widget_get_screen (widget);

  root_window = gdk_screen_get_root_window (screen);

  desktop_icon_view->details->root_window = root_window;

  update_margins (desktop_icon_view);

  /* Setup the property filter */
  gdk_window_set_events (root_window, GDK_PROPERTY_CHANGE_MASK);
  gdk_window_add_filter (root_window,
                         gdk_filter_func,
                         desktop_icon_view);
}

static void
kraken_desktop_icon_view_dispose (GObject *object)
{
	KrakenDesktopIconView *icon_view;
	GtkUIManager *ui_manager;

	icon_view = KRAKEN_DESKTOP_ICON_VIEW (object);

	/* Remove desktop rescan timeout. */
	if (icon_view->details->reload_desktop_timeout != 0) {
		g_source_remove (icon_view->details->reload_desktop_timeout);
		icon_view->details->reload_desktop_timeout = 0;
	}

	ui_manager = kraken_view_get_ui_manager (KRAKEN_VIEW (icon_view));
	if (ui_manager != NULL) {
		kraken_ui_unmerge_ui (ui_manager,
					&icon_view->details->desktop_merge_id,
					&icon_view->details->desktop_action_group);
	}

	g_signal_handlers_disconnect_by_func (kraken_icon_view_preferences,
					      default_zoom_level_changed,
					      icon_view);
	g_signal_handlers_disconnect_by_func (kraken_desktop_preferences,
					      font_changed_callback,
					      icon_view);

	g_signal_handlers_disconnect_by_func (kraken_preferences,
					      desktop_directory_changed_callback,
					      NULL);

	g_signal_handlers_disconnect_by_func (gnome_lockdown_preferences,
					      kraken_view_update_menus,
					      icon_view);

	G_OBJECT_CLASS (kraken_desktop_icon_view_parent_class)->dispose (object);
}

static void
kraken_desktop_icon_view_class_init (KrakenDesktopIconViewClass *class)
{
	KrakenViewClass *vclass;

	vclass = KRAKEN_VIEW_CLASS (class);

    G_OBJECT_CLASS (class)->dispose = kraken_desktop_icon_view_dispose;
	G_OBJECT_CLASS (class)->constructed = kraken_desktop_icon_view_constructed;

    KRAKEN_ICON_VIEW_CLASS (class)->use_grid_container = FALSE;

	vclass->merge_menus = real_merge_menus;
	vclass->update_menus = real_update_menus;
	vclass->get_view_id = real_get_id;
    vclass->add_file = real_add_file;

#if GTK_CHECK_VERSION(3, 21, 0)
	GtkWidgetClass *wclass = GTK_WIDGET_CLASS (class);
	gtk_widget_class_set_css_name (wclass, "kraken-desktop-icon-view");
#endif
	g_type_class_add_private (class, sizeof (KrakenDesktopIconViewDetails));
}

static void
kraken_desktop_icon_view_handle_middle_click (KrakenIconContainer *icon_container,
						GdkEventButton *event,
						KrakenDesktopIconView *desktop_icon_view)
{
	XButtonEvent x_event;
	GdkDevice *keyboard = NULL, *pointer = NULL, *cur;
	GdkDeviceManager *manager;
	GList *list, *l;

	manager = gdk_display_get_device_manager (gtk_widget_get_display (GTK_WIDGET (icon_container)));
	list = gdk_device_manager_list_devices (manager, GDK_DEVICE_TYPE_MASTER);

	for (l = list; l != NULL; l = l->next) {
		cur = l->data;

		if (pointer == NULL && (gdk_device_get_source (cur) == GDK_SOURCE_MOUSE)) {
			pointer = cur;
		}

		if (keyboard == NULL && (gdk_device_get_source (cur) == GDK_SOURCE_KEYBOARD)) {
			keyboard = cur;
		}

		if (pointer != NULL && keyboard != NULL) {
			break;
		}
	}

	g_list_free (list);

	/* During a mouse click we have the pointer and keyboard grab.
	 * We will send a fake event to the root window which will cause it
	 * to try to get the grab so we need to let go ourselves.
	 */

	if (pointer != NULL) {
		gdk_device_ungrab (pointer, GDK_CURRENT_TIME);
	}


	if (keyboard != NULL) {
		gdk_device_ungrab (keyboard, GDK_CURRENT_TIME);
	}

	/* Stop the event because we don't want anyone else dealing with it. */
	gdk_flush ();
	g_signal_stop_emission_by_name (icon_container, "middle_click");

	/* build an X event to represent the middle click. */
	x_event.type = ButtonPress;
	x_event.send_event = True;
	x_event.display = GDK_DISPLAY_XDISPLAY (gdk_display_get_default ());
	x_event.window = GDK_ROOT_WINDOW ();
	x_event.root = GDK_ROOT_WINDOW ();
	x_event.subwindow = 0;
	x_event.time = event->time;
	x_event.x = event->x;
	x_event.y = event->y;
	x_event.x_root = event->x_root;
	x_event.y_root = event->y_root;
	x_event.state = event->state;
	x_event.button = event->button;
	x_event.same_screen = True;

	/* Send it to the root window, the window manager will handle it. */
	XSendEvent (GDK_DISPLAY_XDISPLAY (gdk_display_get_default ()), GDK_ROOT_WINDOW (), True,
		    ButtonPressMask, (XEvent *) &x_event);
}

static void
desktop_icon_container_realize (GtkWidget *widget,
                                KrakenDesktopIconView *desktop_icon_view)
{
    GdkWindow *bin_window;
    GdkRGBA transparent = { 0, 0, 0, 0 };

    bin_window = gtk_layout_get_bin_window (GTK_LAYOUT (widget));
    gdk_window_set_background_rgba (bin_window, &transparent);
}

static KrakenZoomLevel
get_default_zoom_level (void)
{
	KrakenZoomLevel default_zoom_level;

	default_zoom_level = g_settings_get_enum (kraken_icon_view_preferences,
						  KRAKEN_PREFERENCES_ICON_VIEW_DEFAULT_ZOOM_LEVEL);

	return CLAMP (default_zoom_level, KRAKEN_ZOOM_LEVEL_SMALLEST, KRAKEN_ZOOM_LEVEL_LARGEST);
}

static void
default_zoom_level_changed (gpointer user_data)
{
	KrakenZoomLevel new_level;
	KrakenDesktopIconView *desktop_icon_view;

	desktop_icon_view = KRAKEN_DESKTOP_ICON_VIEW (user_data);
	new_level = get_default_zoom_level ();

	kraken_icon_container_set_zoom_level (get_icon_container (desktop_icon_view),
						new_level);
}

static gboolean
do_desktop_rescan (gpointer data)
{
	KrakenDesktopIconView *desktop_icon_view;
	struct stat buf;

	desktop_icon_view = KRAKEN_DESKTOP_ICON_VIEW (data);
	if (desktop_icon_view->details->pending_rescan) {
		return TRUE;
	}

	if (stat (desktop_directory, &buf) == -1) {
		return TRUE;
	}

	if (buf.st_ctime == desktop_dir_modify_time) {
		return TRUE;
	}

	desktop_icon_view->details->pending_rescan = TRUE;

	kraken_directory_force_reload
		(kraken_view_get_model (KRAKEN_VIEW (desktop_icon_view)));

	return TRUE;
}

static void
done_loading (KrakenDirectory *model,
	      KrakenDesktopIconView *desktop_icon_view)
{
	struct stat buf;

	desktop_icon_view->details->pending_rescan = FALSE;
	if (stat (desktop_directory, &buf) == -1) {
		return;
	}

	desktop_dir_modify_time = buf.st_ctime;
}

/* This function is used because the KrakenDirectory model does not
 * exist always in the desktop_icon_view, so we wait until it has been
 * instantiated.
 */
static void
delayed_init (KrakenDesktopIconView *desktop_icon_view)
{
	/* Keep track of the load time. */
	g_signal_connect_object (kraken_view_get_model (KRAKEN_VIEW (desktop_icon_view)),
				 "done_loading",
				 G_CALLBACK (done_loading), desktop_icon_view, 0);

	/* Monitor desktop directory. */
	desktop_icon_view->details->reload_desktop_timeout =
		g_timeout_add_seconds (RESCAN_TIMEOUT, do_desktop_rescan, desktop_icon_view);

	g_signal_handler_disconnect (desktop_icon_view,
				     desktop_icon_view->details->delayed_init_signal);

	desktop_icon_view->details->delayed_init_signal = 0;
}

static void
font_changed_callback (gpointer callback_data)
{
 	g_return_if_fail (KRAKEN_IS_DESKTOP_ICON_VIEW (callback_data));

	kraken_desktop_icon_view_update_icon_container_fonts (KRAKEN_DESKTOP_ICON_VIEW (callback_data));
}

static void
kraken_desktop_icon_view_update_icon_container_fonts (KrakenDesktopIconView *icon_view)
{
	KrakenIconContainer *icon_container;
	char *font;

	icon_container = get_icon_container (icon_view);
	g_assert (icon_container != NULL);

	font = g_settings_get_string (kraken_desktop_preferences,
				      KRAKEN_PREFERENCES_DESKTOP_FONT);

	kraken_icon_container_set_font (icon_container, font);

	g_free (font);
}

static void
kraken_desktop_icon_view_init (KrakenDesktopIconView *desktop_icon_view)
{
    desktop_icon_view->details = G_TYPE_INSTANCE_GET_PRIVATE (desktop_icon_view,
                                                              KRAKEN_TYPE_DESKTOP_ICON_VIEW,
                                                              KrakenDesktopIconViewDetails);
}

static void
kraken_desktop_icon_view_constructed (GObject *object)
{
    KrakenDesktopIconView *desktop_icon_view;
    KrakenIconContainer *icon_container;
    GtkAllocation allocation;
    GtkAdjustment *hadj, *vadj;

    desktop_icon_view = KRAKEN_DESKTOP_ICON_VIEW (object);

    G_OBJECT_CLASS (kraken_desktop_icon_view_parent_class)->constructed (G_OBJECT (desktop_icon_view));

    if (desktop_directory == NULL) {
        g_signal_connect_swapped (kraken_preferences, "changed::" KRAKEN_PREFERENCES_DESKTOP_IS_HOME_DIR,
                      G_CALLBACK(desktop_directory_changed_callback),
                      NULL);
        desktop_directory_changed_callback (NULL);
    }

    icon_container = get_icon_container (desktop_icon_view);
    kraken_icon_container_set_use_drop_shadows (icon_container, TRUE);
    kraken_icon_view_container_set_sort_desktop (KRAKEN_ICON_VIEW_CONTAINER (icon_container), TRUE);

    /* Do a reload on the desktop if we don't have FAM, a smarter
     * way to keep track of the items on the desktop.
     */
    if (!kraken_monitor_active ()) {
        desktop_icon_view->details->delayed_init_signal = g_signal_connect_object
            (desktop_icon_view, "begin_loading",
             G_CALLBACK (delayed_init), desktop_icon_view, 0);
    }

    kraken_icon_container_set_is_fixed_size (icon_container, TRUE);
    kraken_icon_container_set_is_desktop (icon_container, TRUE);

    kraken_icon_container_set_store_layout_timestamps (icon_container, TRUE);

    /* Set allocation to be at 0, 0 */
    gtk_widget_get_allocation (GTK_WIDGET (icon_container), &allocation);
    allocation.x = 0;
    allocation.y = 0;
    gtk_widget_set_allocation (GTK_WIDGET (icon_container), &allocation);

    gtk_widget_queue_resize (GTK_WIDGET (icon_container));

    hadj = gtk_scrollable_get_hadjustment (GTK_SCROLLABLE (icon_container));
    vadj = gtk_scrollable_get_vadjustment (GTK_SCROLLABLE (icon_container));

    gtk_adjustment_set_value (hadj, 0);
    gtk_adjustment_set_value (vadj, 0);

    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (desktop_icon_view),
                         GTK_SHADOW_NONE);

    kraken_view_ignore_hidden_file_preferences
        (KRAKEN_VIEW (desktop_icon_view));

    kraken_view_set_show_foreign (KRAKEN_VIEW (desktop_icon_view),
                    FALSE);

    /* Set our default layout mode */
    kraken_icon_container_set_layout_mode (icon_container,
                         gtk_widget_get_direction (GTK_WIDGET(icon_container)) == GTK_TEXT_DIR_RTL ?
                         KRAKEN_ICON_LAYOUT_T_B_R_L :
                         KRAKEN_ICON_LAYOUT_T_B_L_R);

    g_signal_connect_object (icon_container, "middle_click",
                 G_CALLBACK (kraken_desktop_icon_view_handle_middle_click), desktop_icon_view, 0);
    g_signal_connect_object (icon_container, "realize",
                 G_CALLBACK (desktop_icon_container_realize), desktop_icon_view, 0);

    g_signal_connect_swapped (kraken_icon_view_preferences,
                  "changed::" KRAKEN_PREFERENCES_ICON_VIEW_DEFAULT_ZOOM_LEVEL,
                  G_CALLBACK (default_zoom_level_changed),
                  desktop_icon_view);

    g_signal_connect_object (desktop_icon_view, "realize",
                             G_CALLBACK (realized_callback), desktop_icon_view, 0);
    g_signal_connect_object (desktop_icon_view, "unrealize",
                             G_CALLBACK (unrealized_callback), desktop_icon_view, 0);

    g_signal_connect_swapped (kraken_desktop_preferences,
                  "changed::" KRAKEN_PREFERENCES_DESKTOP_FONT,
                  G_CALLBACK (font_changed_callback),
                  desktop_icon_view);

    default_zoom_level_changed (desktop_icon_view);
    kraken_desktop_icon_view_update_icon_container_fonts (desktop_icon_view);

    g_signal_connect_swapped (gnome_lockdown_preferences,
                  "changed::" KRAKEN_PREFERENCES_LOCKDOWN_COMMAND_LINE,
                  G_CALLBACK (kraken_view_update_menus),
                  desktop_icon_view);
}

static void
action_stretch_callback (GtkAction *action,
                         gpointer callback_data)
{
    g_assert (KRAKEN_IS_ICON_VIEW (callback_data));

    kraken_icon_container_show_stretch_handles (get_icon_container (callback_data));
}

static void
action_unstretch_callback (GtkAction *action,
                           gpointer callback_data)
{
    g_assert (KRAKEN_IS_ICON_VIEW (callback_data));

    kraken_icon_container_unstretch (get_icon_container (callback_data));
}

static void
action_empty_trash_conditional_callback (GtkAction *action,
					 gpointer data)
{
        g_assert (KRAKEN_IS_VIEW (data));

	kraken_file_operations_empty_trash (GTK_WIDGET (data));
}

static gboolean
trash_link_is_selection (KrakenView *view)
{
	GList *selection;
	KrakenDesktopLink *link;
	gboolean result;

	result = FALSE;

	selection = kraken_view_get_selection (view);

	if ((g_list_length (selection) == 1) &&
	    KRAKEN_IS_DESKTOP_ICON_FILE (selection->data)) {
		link = kraken_desktop_icon_file_get_link (KRAKEN_DESKTOP_ICON_FILE (selection->data));
		/* link may be NULL if the link was recently removed (unmounted) */
		if (link != NULL &&
		    kraken_desktop_link_get_link_type (link) == KRAKEN_DESKTOP_LINK_TRASH) {
			result = TRUE;
		}
		if (link) {
			g_object_unref (link);
		}
	}

	kraken_file_list_free (selection);

	return result;
}

static void
real_update_menus (KrakenView *view)
{
    KrakenDesktopIconView *desktop_view;
    KrakenIconContainer *icon_container;
    char *label;
    gint selection_count;

	gboolean include_empty_trash;
	GtkAction *action;

	g_assert (KRAKEN_IS_DESKTOP_ICON_VIEW (view));

	KRAKEN_VIEW_CLASS (kraken_desktop_icon_view_parent_class)->update_menus (view);

	desktop_view = KRAKEN_DESKTOP_ICON_VIEW (view);

	/* Empty Trash */
	include_empty_trash = trash_link_is_selection (view);
	action = gtk_action_group_get_action (desktop_view->details->desktop_action_group,
					      KRAKEN_ACTION_EMPTY_TRASH_CONDITIONAL);
	gtk_action_set_visible (action,
				include_empty_trash);
	if (include_empty_trash) {
		label = g_strdup (_("E_mpty Trash"));
		g_object_set (action , "label", label, NULL);
		gtk_action_set_sensitive (action,
					  !kraken_trash_monitor_is_empty ());
		g_free (label);
	}

    selection_count = kraken_view_get_selection_count (view);
    icon_container = get_icon_container (desktop_view);

    action = gtk_action_group_get_action (desktop_view->details->desktop_action_group,
                                          KRAKEN_ACTION_STRETCH);
    gtk_action_set_sensitive (action,
                              selection_count == 1 &&
                              icon_container != NULL &&
                              !kraken_icon_container_has_stretch_handles (icon_container));
    gtk_action_set_visible (action, TRUE);

    action = gtk_action_group_get_action (desktop_view->details->desktop_action_group,
                                          KRAKEN_ACTION_UNSTRETCH);
    g_object_set (action, "label",
                  (selection_count > 1) ?
                      _("Restore Icons' Original Si_zes")
                    : _("Restore Icon's Original Si_ze"),
                  NULL);
    gtk_action_set_sensitive (action,
                              icon_container != NULL &&
                              kraken_icon_container_is_stretched (icon_container));
    gtk_action_set_visible (action, TRUE);
}

static const GtkActionEntry desktop_view_entries[] = {
    /* name, stock id */         { "Stretch", NULL,
    /* label, accelerator */       N_("Resize Icon..."), NULL,
    /* tooltip */                  N_("Make the selected icon resizable"),
                                 G_CALLBACK (action_stretch_callback) },
    /* name, stock id */         { "Unstretch", NULL,
    /* label, accelerator */       N_("Restore Icons' Original Si_zes"), NULL,
    /* tooltip */                  N_("Restore each selected icon to its original size"),
                                 G_CALLBACK (action_unstretch_callback) },
    /* name, stock id */         { "Empty Trash Conditional", NULL,
    /* label, accelerator */       N_("Empty Trash"), NULL,
    /* tooltip */                  N_("Delete all items in the Trash"),
                                 G_CALLBACK (action_empty_trash_conditional_callback) }
};

static void
real_merge_menus (KrakenView *view)
{
	KrakenDesktopIconView *desktop_view;
	GtkUIManager *ui_manager;
	GtkActionGroup *action_group;

	KRAKEN_VIEW_CLASS (kraken_desktop_icon_view_parent_class)->merge_menus (view);

	desktop_view = KRAKEN_DESKTOP_ICON_VIEW (view);

	ui_manager = kraken_view_get_ui_manager (view);

	action_group = gtk_action_group_new ("DesktopViewActions");
	gtk_action_group_set_translation_domain (action_group, GETTEXT_PACKAGE);
	desktop_view->details->desktop_action_group = action_group;
	gtk_action_group_add_actions (action_group,
				      desktop_view_entries, G_N_ELEMENTS (desktop_view_entries),
				      view);

	gtk_ui_manager_insert_action_group (ui_manager, action_group, 0);
	g_object_unref (action_group); /* owned by ui manager */

	desktop_view->details->desktop_merge_id =
		gtk_ui_manager_add_ui_from_resource (ui_manager, "/org/kraken/kraken-desktop-icon-view-ui.xml", NULL);
}

static KrakenView *
kraken_desktop_icon_view_create (KrakenWindowSlot *slot)
{
	KrakenIconView *view;

	view = g_object_new (KRAKEN_TYPE_DESKTOP_ICON_VIEW,
			     "window-slot", slot,
			     "supports-zooming", FALSE,
			     "supports-auto-layout", FALSE,
			     "is-desktop", TRUE,
			     "supports-keep-aligned", TRUE,
			     "supports-labels-beside-icons", FALSE,
			     NULL);
	return KRAKEN_VIEW (view);
}

static gboolean
kraken_desktop_icon_view_supports_uri (const char *uri,
				   GFileType file_type,
				   const char *mime_type)
{
	if (g_str_has_prefix (uri, EEL_DESKTOP_URI)) {
		return TRUE;
	}

	return FALSE;
}

static KrakenViewInfo kraken_desktop_icon_view = {
	(char *)KRAKEN_DESKTOP_ICON_VIEW_ID,
	(char *)"Desktop View",
	(char *)"_Desktop",
	(char *)N_("The desktop view encountered an error."),
	(char *)N_("The desktop view encountered an error while starting up."),
	(char *)"Display this location with the desktop view.",
	kraken_desktop_icon_view_create,
	kraken_desktop_icon_view_supports_uri
};

void
kraken_desktop_icon_view_register (void)
{
	kraken_desktop_icon_view.error_label = _(kraken_desktop_icon_view.error_label);
	kraken_desktop_icon_view.startup_error_label = _(kraken_desktop_icon_view.startup_error_label);

	kraken_view_factory_register (&kraken_desktop_icon_view);
}

