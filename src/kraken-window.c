/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/*
 *  Kraken
 *
 *  Copyright (C) 1999, 2000, 2004 Red Hat, Inc.
 *  Copyright (C) 1999, 2000, 2001 Eazel, Inc.
 *
 *  Kraken is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  Kraken is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public
 *  License along with this program; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin Street, Suite 500, MA 02110-1335, USA.
 *
 *  Authors: Elliot Lee <sopwith@redhat.com>
 *  	     John Sullivan <sullivan@eazel.com>
 *           Alexander Larsson <alexl@redhat.com>
 */

/* kraken-window.c: Implementation of the main window object */

#include <config.h>

#include "kraken-window-private.h"

#include "kraken-actions.h"
#include "kraken-application.h"
#include "kraken-bookmarks-window.h"
#include "kraken-desktop-window.h"
#include "kraken-location-bar.h"
#include "kraken-mime-actions.h"
#include "kraken-notebook.h"
#include "kraken-places-sidebar.h"
#include "kraken-tree-sidebar.h"
#include "kraken-view-factory.h"
#include "kraken-window-manage-views.h"
#include "kraken-window-bookmarks.h"
#include "kraken-window-slot.h"
#include "kraken-window-menus.h"
#include "kraken-icon-view.h"
#include "kraken-list-view.h"
#include "kraken-statusbar.h"

#include <eel/eel-debug.h>
#include <eel/eel-gtk-extensions.h>
#include <eel/eel-string.h>
#include <eel/eel-vfs-extensions.h>

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gdk/gdkx.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#ifdef HAVE_X11_XF86KEYSYM_H
#include <X11/XF86keysym.h>
#endif
#include <libkraken-private/kraken-file-utilities.h>
#include <libkraken-private/kraken-file-attributes.h>
#include <libkraken-private/kraken-global-preferences.h>
#include <libkraken-private/kraken-metadata.h>
#include <libkraken-private/kraken-clipboard.h>
#include <libkraken-private/kraken-undo.h>
#include <libkraken-private/kraken-search-directory.h>
#include <libkraken-private/kraken-signaller.h>

#define DEBUG_FLAG KRAKEN_DEBUG_WINDOW
#include <libkraken-private/kraken-debug.h>

#include <math.h>
#include <sys/time.h>

#define MAX_TITLE_LENGTH 180

/* Forward and back buttons on the mouse */
static gboolean mouse_extra_buttons = TRUE;
static guint mouse_forward_button = 9;
static guint mouse_back_button = 8;

static void mouse_back_button_changed		     (gpointer                  callback_data);
static void mouse_forward_button_changed	     (gpointer                  callback_data);
static void use_extra_mouse_buttons_changed          (gpointer              callback_data);
static void side_pane_id_changed                    (KrakenWindow            *window);
static void toggle_menubar                          (KrakenWindow            *window,
                                                     gint                   action);
static void kraken_window_reload                      (KrakenWindow            *window);

/* Sanity check: highest mouse button value I could find was 14. 5 is our
 * lower threshold (well-documented to be the one of the button events for the
 * scrollwheel), so it's hardcoded in the functions below. However, if you have
 * a button that registers higher and want to map it, file a bug and
 * we'll move the bar. Makes you wonder why the X guys don't have
 * defined values for these like the XKB stuff, huh?
 */
#define UPPER_MOUSE_LIMIT 14

enum {
	PROP_DISABLE_CHROME = 1,
    PROP_SIDEBAR_VIEW_TYPE,
    PROP_SHOW_SIDEBAR,
	NUM_PROPERTIES,
};

enum {
	GO_UP,
	RELOAD,
	PROMPT_FOR_LOCATION,
	LOADING_URI,
	HIDDEN_FILES_MODE_CHANGED,
	SLOT_ADDED,
	SLOT_REMOVED,
	LAST_SIGNAL
};

enum {
    MENU_HIDE,
    MENU_SHOW,
    MENU_TOGGLE
};

static guint signals[LAST_SIGNAL] = { 0 };
static GParamSpec *properties[NUM_PROPERTIES] = { NULL, };

G_DEFINE_TYPE (KrakenWindow, kraken_window, GTK_TYPE_APPLICATION_WINDOW);

static const struct {
	unsigned int keyval;
	const char *action;
} extra_window_keybindings [] = {
#ifdef HAVE_X11_XF86KEYSYM_H
	{ XF86XK_AddFavorite,	KRAKEN_ACTION_ADD_BOOKMARK },
	{ XF86XK_Favorites,	KRAKEN_ACTION_EDIT_BOOKMARKS },
	{ XF86XK_Go,		KRAKEN_ACTION_EDIT_LOCATION },
	{ XF86XK_HomePage,      KRAKEN_ACTION_GO_HOME },
	{ XF86XK_OpenURL,	KRAKEN_ACTION_EDIT_LOCATION },
	{ XF86XK_Refresh,	KRAKEN_ACTION_RELOAD },
	{ XF86XK_Reload,	KRAKEN_ACTION_RELOAD },
	{ XF86XK_Search,	KRAKEN_ACTION_SEARCH },
	{ XF86XK_Start,		KRAKEN_ACTION_GO_HOME },
	{ XF86XK_Stop,		KRAKEN_ACTION_STOP },
	{ XF86XK_ZoomIn,	KRAKEN_ACTION_ZOOM_IN },
	{ XF86XK_ZoomOut,	KRAKEN_ACTION_ZOOM_OUT },
	{ XF86XK_Back,		KRAKEN_ACTION_BACK },
	{ XF86XK_Forward,	KRAKEN_ACTION_FORWARD }

#endif
};

void
kraken_window_push_status (KrakenWindow *window,
			     const char *text)
{
	g_return_if_fail (KRAKEN_IS_WINDOW (window));

	/* clear any previous message, underflow is allowed */
	gtk_statusbar_pop (GTK_STATUSBAR (window->details->statusbar), 0);

	if (text != NULL && text[0] != '\0') {
		gtk_statusbar_push (GTK_STATUSBAR (window->details->statusbar), 0, text);
	}
}

void
kraken_window_go_to (KrakenWindow *window, GFile *location)
{
	g_return_if_fail (KRAKEN_IS_WINDOW (window));

	kraken_window_slot_open_location (kraken_window_get_active_slot (window),
					    location, 0);
}

void
kraken_window_go_to_tab (KrakenWindow *window, GFile *location)
{
	g_return_if_fail (KRAKEN_IS_WINDOW (window));

	kraken_window_slot_open_location (kraken_window_get_active_slot (window),
					    location, KRAKEN_WINDOW_OPEN_FLAG_NEW_TAB);
}

void
kraken_window_go_to_full (KrakenWindow *window,
			    GFile          *location,
			    KrakenWindowGoToCallback callback,
			    gpointer        user_data)
{
	g_return_if_fail (KRAKEN_IS_WINDOW (window));

	kraken_window_slot_open_location_full (kraken_window_get_active_slot (window),
						 location, 0, NULL, callback, user_data);
}

static void
kraken_window_go_up_signal (KrakenWindow *window)
{
	kraken_window_slot_go_up (kraken_window_get_active_slot (window), 0);
}

void
kraken_window_slot_removed (KrakenWindow *window,  KrakenWindowSlot *slot)
{
	g_signal_emit (window, signals[SLOT_REMOVED], 0, slot);
}

void
kraken_window_slot_added (KrakenWindow *window,  KrakenWindowSlot *slot)
{
    g_signal_emit (window, signals[SLOT_ADDED], 0, slot);
}

void
kraken_window_new_tab (KrakenWindow *window)
{
	KrakenWindowSlot *current_slot;
	KrakenWindowSlot *new_slot;
	KrakenWindowOpenFlags flags;
	GFile *location;
	int new_slot_position;
	char *scheme;

	current_slot = kraken_window_get_active_slot (window);
	location = kraken_window_slot_get_location (current_slot);

	if (location != NULL) {
		flags = 0;

		new_slot_position = g_settings_get_enum (kraken_preferences, KRAKEN_PREFERENCES_NEW_TAB_POSITION);
		if (new_slot_position == KRAKEN_NEW_TAB_POSITION_END) {
			flags = KRAKEN_WINDOW_OPEN_SLOT_APPEND;
		}

		scheme = g_file_get_uri_scheme (location);
		if (!strcmp (scheme, "x-kraken-search")) {
			g_object_unref (location);
			location = g_file_new_for_path (g_get_home_dir ());
		}
		g_free (scheme);

		new_slot = kraken_window_pane_open_slot (current_slot->pane, flags);
		kraken_window_set_active_slot (window, new_slot);
		kraken_window_slot_open_location (new_slot, location, 0);
		g_object_unref (location);
	}
}

static void
update_cursor (KrakenWindow *window)
{
	KrakenWindowSlot *slot;
	GdkCursor *cursor;

	slot = kraken_window_get_active_slot (window);

	if (slot && slot->allow_stop) {
		cursor = gdk_cursor_new (GDK_WATCH);
                gdk_window_set_cursor (gtk_widget_get_window (GTK_WIDGET (window)), cursor);
		g_object_unref (cursor);
	} else {
                gdk_window_set_cursor (gtk_widget_get_window (GTK_WIDGET (window)), NULL);
        }
}

void
kraken_window_sync_allow_stop (KrakenWindow *window,
				 KrakenWindowSlot *slot)
{
	GtkAction *stop_action;
	GtkAction *reload_action;
	gboolean allow_stop, slot_is_active;
	KrakenNotebook *notebook;

	stop_action = gtk_action_group_get_action (kraken_window_get_main_action_group (window),
						   KRAKEN_ACTION_STOP);
	reload_action = gtk_action_group_get_action (kraken_window_get_main_action_group (window),
						     KRAKEN_ACTION_RELOAD);
	allow_stop = gtk_action_get_sensitive (stop_action);

	slot_is_active = (slot == kraken_window_get_active_slot (window));

	if (!slot_is_active ||
	    allow_stop != slot->allow_stop) {
		if (slot_is_active) {
			gtk_action_set_visible (stop_action, slot->allow_stop);
			gtk_action_set_visible (reload_action, !slot->allow_stop);
		}

		if (gtk_widget_get_realized (GTK_WIDGET (window))) {
			update_cursor (window);
		}


		notebook = KRAKEN_NOTEBOOK (slot->pane->notebook);
		kraken_notebook_sync_loading (notebook, slot);
	}
}

static void
kraken_window_prompt_for_location (KrakenWindow *window,
                                 const char *initial)
{
    KrakenWindowPane *pane;

    g_return_if_fail (KRAKEN_IS_WINDOW (window));

    if (!KRAKEN_IS_DESKTOP_WINDOW (window)) {
        if (initial) {
            KrakenEntry *entry;
            kraken_window_show_location_entry(window);
            pane = window->details->active_pane;
            entry = kraken_location_bar_get_entry (KRAKEN_LOCATION_BAR (pane->location_bar));
            kraken_entry_set_text (entry, initial);
            gtk_editable_set_position (GTK_EDITABLE (entry), -1);
        }
    }
}

/* Code should never force the window taller than this size.
 * (The user can still stretch the window taller if desired).
 */
static guint
get_max_forced_height (GdkScreen *screen)
{
	return (gdk_screen_get_height (screen) * 90) / 100;
}

/* Code should never force the window wider than this size.
 * (The user can still stretch the window wider if desired).
 */
static guint
get_max_forced_width (GdkScreen *screen)
{
	return (gdk_screen_get_width (screen) * 90) / 100;
}

/* This must be called when construction of KrakenWindow is finished,
 * since it depends on the type of the argument, which isn't decided at
 * construction time.
 */
static void
kraken_window_set_initial_window_geometry (KrakenWindow *window)
{
	GdkScreen *screen;
	guint max_width_for_screen, max_height_for_screen;
	guint default_width, default_height;

	screen = gtk_window_get_screen (GTK_WINDOW (window));

	max_width_for_screen = get_max_forced_width (screen);
	max_height_for_screen = get_max_forced_height (screen);

	default_width = KRAKEN_WINDOW_DEFAULT_WIDTH;
	default_height = KRAKEN_WINDOW_DEFAULT_HEIGHT;

	gtk_window_set_default_size (GTK_WINDOW (window),
				     MIN (default_width,
				          max_width_for_screen),
				     MIN (default_height,
				          max_height_for_screen));
}

static gboolean
save_sidebar_width_cb (gpointer user_data)
{
	KrakenWindow *window = user_data;

	window->details->sidebar_width_handler_id = 0;

	DEBUG ("Saving sidebar width: %d", window->details->side_pane_width);

	g_settings_set_int (kraken_window_state,
			    KRAKEN_WINDOW_STATE_SIDEBAR_WIDTH,
			    window->details->side_pane_width);

	return FALSE;
}

/* side pane helpers */
static void
side_pane_size_allocate_callback (GtkWidget *widget,
				  GtkAllocation *allocation,
				  gpointer user_data)
{
	KrakenWindow *window;

	window = user_data;

	if (window->details->sidebar_width_handler_id != 0) {
		g_source_remove (window->details->sidebar_width_handler_id);
		window->details->sidebar_width_handler_id = 0;
	}

	if (allocation->width != window->details->side_pane_width &&
	    allocation->width > 1) {
		window->details->side_pane_width = allocation->width;

		window->details->sidebar_width_handler_id =
			g_timeout_add (100, save_sidebar_width_cb, window);
	}
}

static void
setup_side_pane_width (KrakenWindow *window)
{
	g_return_if_fail (window->details->sidebar != NULL);

	window->details->side_pane_width =
		g_settings_get_int (kraken_window_state,
				    KRAKEN_WINDOW_STATE_SIDEBAR_WIDTH);

	gtk_paned_set_position (GTK_PANED (window->details->content_paned),
				window->details->side_pane_width);
}

static void
kraken_window_set_up_sidebar (KrakenWindow *window)
{
	GtkWidget *sidebar;

	DEBUG ("Setting up sidebar id %s", window->details->sidebar_id);

	window->details->sidebar = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_style_context_add_class (gtk_widget_get_style_context (window->details->sidebar),
				     GTK_STYLE_CLASS_SIDEBAR);

	gtk_paned_pack1 (GTK_PANED (window->details->content_paned),
			 GTK_WIDGET (window->details->sidebar),
			 FALSE, FALSE);

	setup_side_pane_width (window);
	g_signal_connect (window->details->sidebar,
			  "size_allocate",
			  G_CALLBACK (side_pane_size_allocate_callback),
			  window);

    g_signal_connect_object (KRAKEN_WINDOW (window), "notify::sidebar-view-id",
                             G_CALLBACK (side_pane_id_changed), window, 0);

    if (g_strcmp0 (window->details->sidebar_id, KRAKEN_WINDOW_SIDEBAR_PLACES) == 0) {
        sidebar = kraken_places_sidebar_new (window);
    } else if (g_strcmp0 (window->details->sidebar_id, KRAKEN_WINDOW_SIDEBAR_TREE) == 0) {
        sidebar = kraken_tree_sidebar_new (window);
    } else {
        g_assert_not_reached ();
    }

	gtk_box_pack_start (GTK_BOX (window->details->sidebar), sidebar, TRUE, TRUE, 0);
	gtk_widget_show (sidebar);
	gtk_widget_show (GTK_WIDGET (window->details->sidebar));
}

static void
kraken_window_tear_down_sidebar (KrakenWindow *window)
{
	DEBUG ("Destroying sidebar");

    g_signal_handlers_disconnect_by_func (KRAKEN_WINDOW (window), side_pane_id_changed, window);

	if (window->details->sidebar != NULL) {
		gtk_widget_destroy (GTK_WIDGET (window->details->sidebar));
		window->details->sidebar = NULL;
	}
}

void
kraken_window_hide_sidebar (KrakenWindow *window)
{
	DEBUG ("Called hide_sidebar()");

	if (window->details->sidebar == NULL) {
		return;
	}

	kraken_window_tear_down_sidebar (window);
	kraken_window_update_show_hide_ui_elements (window);

    kraken_window_set_show_sidebar (window, FALSE);
}

void
kraken_window_show_sidebar (KrakenWindow *window)
{
	DEBUG ("Called show_sidebar()");

	if (window->details->sidebar != NULL) {
		return;
	}

	if (window->details->disable_chrome) {
		return;
	}

	kraken_window_set_up_sidebar (window);
	kraken_window_update_show_hide_ui_elements (window);

    kraken_window_set_show_sidebar (window, TRUE);
}

static gboolean
sidebar_id_is_valid (const gchar *sidebar_id)
{
    return (g_strcmp0 (sidebar_id, KRAKEN_WINDOW_SIDEBAR_PLACES) == 0 ||
            g_strcmp0 (sidebar_id, KRAKEN_WINDOW_SIDEBAR_TREE) == 0);
}

static void
side_pane_id_changed (KrakenWindow *window)
{

    if (!sidebar_id_is_valid (window->details->sidebar_id)) {
        return;
    }

    /* refresh the sidebar setting */
    kraken_window_tear_down_sidebar (window);
    kraken_window_set_up_sidebar (window);
}

gboolean
kraken_window_disable_chrome_mapping (GValue *value,
					GVariant *variant,
					gpointer user_data)
{
	KrakenWindow *window = user_data;

	g_value_set_boolean (value,
			     g_variant_get_boolean (variant) &&
			     !window->details->disable_chrome);

	return TRUE;
}

static gboolean
on_button_press_callback (GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
    KrakenWindow *window = KRAKEN_WINDOW (user_data);

    if (event->button == 3) {
        toggle_menubar (window, MENU_TOGGLE);
    }

    return GDK_EVENT_STOP;
}

static void
clear_menu_hide_delay (KrakenWindow *window)
{
    if (window->details->menu_hide_delay_id > 0) {
        g_source_remove (window->details->menu_hide_delay_id);
    }

    window->details->menu_hide_delay_id = 0;
}

static gboolean
hide_menu_on_delay (KrakenWindow *window)
{
    toggle_menubar (window, MENU_HIDE);

    window->details->menu_hide_delay_id = 0;
    return FALSE;
}

static gboolean
on_menu_focus_out (GtkMenuShell *widget,
                   GdkEvent  *event,
                   gpointer   user_data)
{
    KrakenWindow *window = KRAKEN_WINDOW (user_data);

    /* The menu, when visible on demand, gets the keyboard grab.
     * If the user clicks on some element in the window,, we want the menu
     * to disappear, but if it's done immediately, everything shifts up the
     * height of the menu, and the user will more than likely end up clicking
     * in the wrong spot.  Delay the hide momentarily, to allow the user to
     * complete their click action. */
    clear_menu_hide_delay (window);

    /* When a submenu pops-up, the menu loses focus. The menu should disappear
     * only when none of its elements is selected. */
    if (!gtk_menu_shell_get_selected_item (widget)) {
        window->details->menu_hide_delay_id = g_timeout_add (200, (GSourceFunc) hide_menu_on_delay, window);
    }

    return GDK_EVENT_PROPAGATE;
}

void
on_menu_selection_done (GtkMenuShell *menushell,
                        gpointer      user_data)
{
	KrakenWindow *window = KRAKEN_WINDOW (user_data);

	/* Remove the menu inmediately after selecting an item. */
	clear_menu_hide_delay (window);
	window->details->menu_hide_delay_id = g_timeout_add (0, (GSourceFunc) hide_menu_on_delay, window);
}

static void
kraken_window_constructed (GObject *self)
{
	KrakenWindow *window;
	GtkWidget *grid;
	GtkWidget *menu;
	GtkWidget *hpaned;
	GtkWidget *vbox;
	GtkWidget *toolbar_holder;
    GtkWidget *kraken_statusbar;
	KrakenWindowPane *pane;
	KrakenWindowSlot *slot;
	KrakenApplication *application;

	window = KRAKEN_WINDOW (self);
	application = kraken_application_get_singleton ();

	G_OBJECT_CLASS (kraken_window_parent_class)->constructed (self);
	gtk_window_set_application (GTK_WINDOW (window), GTK_APPLICATION (application));

	/* disable automatic menubar handling, since we show our regular
	 * menubar together with the app menu.
	 */
	gtk_application_window_set_show_menubar (GTK_APPLICATION_WINDOW (self), FALSE);

	grid = gtk_grid_new ();
	gtk_orientable_set_orientation (GTK_ORIENTABLE (grid), GTK_ORIENTATION_VERTICAL);
	gtk_widget_show (grid);
	gtk_container_add (GTK_CONTAINER (window), grid);

	/* Statusbar is packed in the subclasses */

	kraken_window_initialize_menus (window);
	kraken_window_initialize_actions (window);

	menu = gtk_ui_manager_get_widget (window->details->ui_manager, "/MenuBar");
	window->details->menubar = menu;

    gtk_widget_set_can_focus (menu, TRUE);
	gtk_widget_set_hexpand (menu, TRUE);

    g_signal_connect_object (menu,
                             "focus-out-event",
                             G_CALLBACK (on_menu_focus_out),
                             window,
                             0);

    g_signal_connect_object (menu,
                             "selection-done",
                             G_CALLBACK (on_menu_selection_done),
                             window,
                             0);

	if (g_settings_get_boolean (kraken_window_state, KRAKEN_WINDOW_STATE_START_WITH_MENU_BAR)){
		gtk_widget_show (menu);
	} else {
		gtk_widget_hide (menu);
	}

    g_settings_bind_with_mapping (kraken_window_state,
                      KRAKEN_WINDOW_STATE_START_WITH_MENU_BAR,
                      window->details->menubar,
                      "visible",
                      G_SETTINGS_BIND_GET,
                      kraken_window_disable_chrome_mapping, NULL,
                      window, NULL);

	gtk_container_add (GTK_CONTAINER (grid), menu);

	/* Set up the toolbar place holder */
	toolbar_holder = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_container_add (GTK_CONTAINER (grid), toolbar_holder);
	gtk_widget_show (toolbar_holder);

    g_signal_connect_object (toolbar_holder, "button-press-event",
                             G_CALLBACK (on_button_press_callback), window, 0);

	window->details->toolbar_holder = toolbar_holder;

	/* Register to menu provider extension signal managing menu updates */
	g_signal_connect_object (kraken_signaller_get_current (), "popup_menu_changed",
			 G_CALLBACK (kraken_window_load_extension_menus), window, G_CONNECT_SWAPPED);

	window->details->content_paned = gtk_paned_new (GTK_ORIENTATION_HORIZONTAL);
	gtk_widget_set_hexpand (window->details->content_paned, TRUE);
	gtk_widget_set_vexpand (window->details->content_paned, TRUE);

	gtk_container_add (GTK_CONTAINER (grid), window->details->content_paned);
	gtk_widget_show (window->details->content_paned);

	vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_paned_pack2 (GTK_PANED (window->details->content_paned), vbox,
			 TRUE, FALSE);
	gtk_widget_show (vbox);

	hpaned = gtk_paned_new (GTK_ORIENTATION_HORIZONTAL);
	gtk_box_pack_start (GTK_BOX (vbox), hpaned, TRUE, TRUE, 0);
	gtk_widget_show (hpaned);
	window->details->split_view_hpane = hpaned;

	pane = kraken_window_pane_new (window);
	window->details->panes = g_list_prepend (window->details->panes, pane);

	gtk_paned_pack1 (GTK_PANED (hpaned), GTK_WIDGET (pane), TRUE, FALSE);


    kraken_statusbar = kraken_status_bar_new (window);
    window->details->kraken_status_bar = kraken_statusbar;

    GtkWidget *sep = gtk_separator_new (GTK_ORIENTATION_HORIZONTAL);
    gtk_container_add (GTK_CONTAINER (grid), sep);
    gtk_widget_show (sep);

    GtkWidget *eb;

    eb = gtk_event_box_new ();
    gtk_container_add (GTK_CONTAINER (eb), kraken_statusbar);
    gtk_container_add (GTK_CONTAINER (grid), eb);
    gtk_widget_show (eb);

    window->details->statusbar = kraken_status_bar_get_real_statusbar (KRAKEN_STATUS_BAR (kraken_statusbar));
    window->details->help_message_cid = gtk_statusbar_get_context_id (GTK_STATUSBAR (window->details->statusbar),
                                                                      "help_message");

    gtk_widget_add_events (GTK_WIDGET (eb), GDK_BUTTON_PRESS_MASK);

    g_signal_connect_object (GTK_WIDGET (eb), "button-press-event",
                             G_CALLBACK (on_button_press_callback), window, 0);

    g_settings_bind_with_mapping (kraken_window_state,
                      KRAKEN_WINDOW_STATE_START_WITH_STATUS_BAR,
                      window->details->kraken_status_bar,
                      "visible",
                      G_SETTINGS_BIND_DEFAULT,
                      kraken_window_disable_chrome_mapping, NULL,
                      window, NULL);

    g_object_bind_property (window->details->kraken_status_bar, "visible",
                            sep, "visible",
                            G_BINDING_DEFAULT | G_BINDING_SYNC_CREATE);

	/* this has to be done after the location bar has been set up,
	 * but before menu stuff is being called */
	kraken_window_set_active_pane (window, pane);

	side_pane_id_changed (window);

	kraken_window_initialize_bookmarks_menu (window);
	kraken_window_set_initial_window_geometry (window);

	slot = kraken_window_pane_open_slot (window->details->active_pane, 0);
	kraken_window_set_active_slot (window, slot);

    if (g_settings_get_boolean (kraken_preferences, KRAKEN_PREFERENCES_START_WITH_DUAL_PANE) &&
        !window->details->disable_chrome)
        kraken_window_split_view_on (window);

    g_signal_connect_swapped (GTK_WINDOW (window),
                              "notify::scale-factor",
                              G_CALLBACK (kraken_window_reload),
                              window);
}

static void
kraken_window_set_property (GObject *object,
			      guint arg_id,
			      const GValue *value,
			      GParamSpec *pspec)
{
	KrakenWindow *window;

	window = KRAKEN_WINDOW (object);

	switch (arg_id) {
	case PROP_DISABLE_CHROME:
		window->details->disable_chrome = g_value_get_boolean (value);
		break;
    case PROP_SIDEBAR_VIEW_TYPE:
        window->details->sidebar_id = g_strdup (g_value_get_string (value));
        break;
    case PROP_SHOW_SIDEBAR:
        kraken_window_set_show_sidebar (window, g_value_get_boolean (value));
        break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, arg_id, pspec);
		break;
	}
}

static void
kraken_window_get_property (GObject *object,
			      guint arg_id,
			      GValue *value,
			      GParamSpec *pspec)
{
	KrakenWindow *window;

	window = KRAKEN_WINDOW (object);

	switch (arg_id) {
        case PROP_DISABLE_CHROME:
            g_value_set_boolean (value, window->details->disable_chrome);
            break;
        case PROP_SIDEBAR_VIEW_TYPE:
            g_value_set_string (value, window->details->sidebar_id);
            break;
        case PROP_SHOW_SIDEBAR:
            g_value_set_boolean (value, window->details->show_sidebar);
            break;
        default:
        	g_assert_not_reached ();
        	break;
	}
}

static void
destroy_panes_foreach (gpointer data,
		       gpointer user_data)
{
	KrakenWindowPane *pane = data;
	KrakenWindow *window = user_data;

	kraken_window_close_pane (window, pane);
}

static void
kraken_window_destroy (GtkWidget *object)
{
	KrakenWindow *window;
	GList *panes_copy;

	window = KRAKEN_WINDOW (object);

	DEBUG ("Destroying window");

	/* close the sidebar first */
	kraken_window_tear_down_sidebar (window);

	/* close all panes safely */
	panes_copy = g_list_copy (window->details->panes);
	g_list_foreach (panes_copy, (GFunc) destroy_panes_foreach, window);
	g_list_free (panes_copy);

	/* the panes list should now be empty */
	g_assert (window->details->panes == NULL);
	g_assert (window->details->active_pane == NULL);

	GTK_WIDGET_CLASS (kraken_window_parent_class)->destroy (object);
}

static void
kraken_window_finalize (GObject *object)
{
	KrakenWindow *window;

	window = KRAKEN_WINDOW (object);

	if (window->details->sidebar_width_handler_id != 0) {
		g_source_remove (window->details->sidebar_width_handler_id);
		window->details->sidebar_width_handler_id = 0;
	}

    g_signal_handlers_disconnect_by_func (kraken_preferences,
                                          kraken_window_sync_thumbnail_action,
                                          window);

    clear_menu_hide_delay (window);

	kraken_window_finalize_menus (window);

	g_clear_object (&window->details->nav_state);
    g_clear_object (&window->details->secondary_pane_last_location);

	g_clear_object (&window->details->ui_manager);

	g_free (window->details->sidebar_id);

	/* kraken_window_close() should have run */
	g_assert (window->details->panes == NULL);

	G_OBJECT_CLASS (kraken_window_parent_class)->finalize (object);
}

void
kraken_window_view_visible (KrakenWindow *window,
			      KrakenView *view)
{
	KrakenWindowSlot *slot;
	KrakenWindowPane *pane;
	GList *l, *walk;

	g_return_if_fail (KRAKEN_IS_WINDOW (window));

	slot = kraken_window_get_slot_for_view (window, view);

	if (slot->visible) {
		return;
	}

	slot->visible = TRUE;
	pane = slot->pane;

	if (gtk_widget_get_visible (GTK_WIDGET (pane))) {
		return;
	}

	/* Look for other non-visible slots */
	for (l = pane->slots; l != NULL; l = l->next) {
		slot = l->data;

		if (!slot->visible) {
			return;
		}
	}

	/* None, this pane is visible */
	gtk_widget_show (GTK_WIDGET (pane));

	/* Look for other non-visible panes */
	for (walk = window->details->panes; walk; walk = walk->next) {
		pane = walk->data;

		if (!gtk_widget_get_visible (GTK_WIDGET (pane))) {
			return;
		}

		for (l = pane->slots; l != NULL; l = l->next) {
			slot = l->data;

			kraken_window_slot_update_title (slot);
			kraken_window_slot_update_icon (slot);
		}
	}

	kraken_window_pane_grab_focus (window->details->active_pane);

	/* All slots and panes visible, show window */
	gtk_widget_show (GTK_WIDGET (window));
}

static gboolean
kraken_window_is_desktop (KrakenWindow *window)
{
    return window->details->disable_chrome;
}

static void
kraken_window_save_geometry (KrakenWindow *window)
{
	char *geometry_string;
	gboolean is_maximized;

	g_assert (KRAKEN_IS_WINDOW (window));

	if (gtk_widget_get_window (GTK_WIDGET (window)) && !kraken_window_is_desktop (window)) {
        GdkWindowState state = gdk_window_get_state (gtk_widget_get_window (GTK_WIDGET (window)));

        if (state & GDK_WINDOW_STATE_TILED) {
            return;
        }

        geometry_string = eel_gtk_window_get_geometry_string (GTK_WINDOW (window));

		is_maximized = state & GDK_WINDOW_STATE_MAXIMIZED;

		if (!is_maximized) {
			g_settings_set_string
				(kraken_window_state, KRAKEN_WINDOW_STATE_GEOMETRY,
				 geometry_string);
		}
		g_free (geometry_string);

		g_settings_set_boolean
			(kraken_window_state, KRAKEN_WINDOW_STATE_MAXIMIZED,
			 is_maximized);
	}
}

void
kraken_window_close (KrakenWindow *window)
{
	KRAKEN_WINDOW_CLASS (G_OBJECT_GET_CLASS (window))->close (window);
}

void
kraken_window_close_pane (KrakenWindow *window,
			    KrakenWindowPane *pane)
{
	g_assert (KRAKEN_IS_WINDOW_PANE (pane));

	while (pane->slots != NULL) {
		KrakenWindowSlot *slot = pane->slots->data;

		kraken_window_pane_remove_slot_unsafe (pane, slot);
	}

	/* If the pane was active, set it to NULL. The caller is responsible
	 * for setting a new active pane with kraken_window_set_active_pane()
	 * if it wants to continue using the window. */
	if (window->details->active_pane == pane) {
		window->details->active_pane = NULL;
	}

	/* Required really. Destroying the KrakenWindowPane still leaves behind the toolbar.
	 * This kills it off. Do it before we call gtk_widget_destroy for safety. */
	gtk_container_remove (GTK_CONTAINER (window->details->toolbar_holder), GTK_WIDGET (pane->tool_bar));

	window->details->panes = g_list_remove (window->details->panes, pane);

	gtk_widget_destroy (GTK_WIDGET (pane));
}

KrakenWindowPane*
kraken_window_get_active_pane (KrakenWindow *window)
{
	g_assert (KRAKEN_IS_WINDOW (window));
	return window->details->active_pane;
}

static void
real_set_active_pane (KrakenWindow *window, KrakenWindowPane *new_pane)
{
	/* make old pane inactive, and new one active.
	 * Currently active pane may be NULL (after init). */
	if (window->details->active_pane &&
	    window->details->active_pane != new_pane) {
		kraken_window_pane_set_active (window->details->active_pane, FALSE);
	}
	kraken_window_pane_set_active (new_pane, TRUE);

	window->details->active_pane = new_pane;
}

/* Make the given pane the active pane of its associated window. This
 * always implies making the containing active slot the active slot of
 * the window. */
void
kraken_window_set_active_pane (KrakenWindow *window,
				 KrakenWindowPane *new_pane)
{
	g_assert (KRAKEN_IS_WINDOW_PANE (new_pane));

	DEBUG ("Setting new pane %p as active", new_pane);

	if (new_pane->active_slot) {
		kraken_window_set_active_slot (window, new_pane->active_slot);
	} else if (new_pane != window->details->active_pane) {
		real_set_active_pane (window, new_pane);
	}
}

/* Make both, the given slot the active slot and its corresponding
 * pane the active pane of the associated window.
 * new_slot may be NULL. */
void
kraken_window_set_active_slot (KrakenWindow *window, KrakenWindowSlot *new_slot)
{
	KrakenWindowSlot *old_slot;
	g_assert (KRAKEN_IS_WINDOW (window));

	DEBUG ("Setting new slot %p as active", new_slot);

	if (new_slot) {
		g_assert ((window == kraken_window_slot_get_window (new_slot)));
		g_assert (KRAKEN_IS_WINDOW_PANE (new_slot->pane));
		g_assert (g_list_find (new_slot->pane->slots, new_slot) != NULL);
	}

	old_slot = kraken_window_get_active_slot (window);

	if (old_slot == new_slot) {
		return;
	}

	/* make old slot inactive if it exists (may be NULL after init, for example) */
	if (old_slot != NULL) {
		/* inform window */
		if (old_slot->content_view != NULL) {
			kraken_window_disconnect_content_view (window, old_slot->content_view);
		}
		gtk_widget_hide (GTK_WIDGET (old_slot->pane->tool_bar));
		/* inform slot & view */
		g_signal_emit_by_name (old_slot, "inactive");
	}

	/* deal with panes */
	if (new_slot &&
	    new_slot->pane != window->details->active_pane) {
		real_set_active_pane (window, new_slot->pane);
	}

	window->details->active_pane->active_slot = new_slot;

	/* make new slot active, if it exists */
	if (new_slot) {
		/* inform sidebar panels */
                kraken_window_report_location_change (window);
		/* TODO decide whether "selection-changed" should be emitted */

		if (new_slot->content_view != NULL) {
                        /* inform window */
                        kraken_window_connect_content_view (window, new_slot->content_view);
                }

		// Show active toolbar
		gboolean show_toolbar;
		show_toolbar = g_settings_get_boolean (kraken_window_state, KRAKEN_WINDOW_STATE_START_WITH_TOOLBAR);

		if ( show_toolbar) {
			gtk_widget_show (GTK_WIDGET (new_slot->pane->tool_bar));
		}

		/* inform slot & view */
                g_signal_emit_by_name (new_slot, "active");
	}
}

static void
kraken_window_realize (GtkWidget *widget)
{
	GTK_WIDGET_CLASS (kraken_window_parent_class)->realize (widget);
	update_cursor (KRAKEN_WINDOW (widget));
}

static void
toggle_menubar (KrakenWindow *window, gint action)
{
    GtkWidget *menu;
    gboolean default_visible;

    default_visible = g_settings_get_boolean (kraken_window_state,
                                              KRAKEN_WINDOW_STATE_START_WITH_MENU_BAR);

    if (default_visible || window->details->disable_chrome) {
        return;
    }

    menu = window->details->menubar;

    if (action == MENU_TOGGLE) {
        action = gtk_widget_get_visible (menu) ? MENU_HIDE : MENU_SHOW;
    }

    if (action == MENU_HIDE) {
        gtk_widget_hide (menu);
    } else {
        gtk_widget_show (menu);

        /* When the menu is normally hidden, have an activation of it trigger a key grab.
         * For keyboard users, this is a natural progression, that they will type a mnemonic
         * next to open a menu.  Any loss of focus or click elsewhere will re-hide the menu
         * and cancel focus.
         */
        gtk_widget_grab_focus (menu);
        gtk_window_set_mnemonics_visible (GTK_WINDOW (window), TRUE);
    }

    return;
}

static gboolean
is_alt_key_event (GdkEventKey *event)
{
    GdkModifierType nominal_state;
    gboolean state_ok;

    nominal_state = event->state & gtk_accelerator_get_default_mod_mask();

    /* A key press of alt will show just the alt keyval (GDK_KEY_Alt_L/R).  A key release
     * of a single modifier is always modified by itself.  So a valid press state is 0 and
     * a valid release state is GDK_MOD1_MASK (alt modifier).
     */
    state_ok = (event->type == GDK_KEY_PRESS && nominal_state == 0) ||
               (event->type == GDK_KEY_RELEASE && nominal_state == GDK_MOD1_MASK);

    if (state_ok && (event->keyval == GDK_KEY_Alt_L || event->keyval == GDK_KEY_Alt_R)) {
        return TRUE;
    }

    return FALSE;
}

static gboolean
kraken_window_key_press_event (GtkWidget *widget,
				 GdkEventKey *event)
{
	KrakenWindow *window;
	KrakenWindowSlot *active_slot;
	KrakenView *view;
	GtkWidget *focus_widget;
	size_t i;

	window = KRAKEN_WINDOW (widget);

	active_slot = kraken_window_get_active_slot (window);
	view = active_slot->content_view;

      /**
       * Disable the GTK Emoji Chooser
       */
      if ((event->keyval == GDK_KEY_semicolon || event->keyval == GDK_KEY_period) && (event->state & GDK_CONTROL_MASK)) {
          return FALSE;
      }

	if (view != NULL && kraken_view_get_is_renaming (view) && event->keyval != GDK_KEY_F2) {
		/* if we're renaming, just forward the event to the
		 * focused widget and return. We don't want to process the window
		 * accelerator bindings, as they might conflict with the
		 * editable widget bindings.
		 */
		if (gtk_window_propagate_key_event (GTK_WINDOW (window), event)) {
			return TRUE;
		}

               /* Do not allow for other accelerator bindings to fire off while
                *  renaming is in progress
                */
               return FALSE;
	}

	focus_widget = gtk_window_get_focus (GTK_WINDOW (window));
	if (view != NULL && focus_widget != NULL &&
	    GTK_IS_EDITABLE (focus_widget)) {
		/* if we have input focus on a GtkEditable (e.g. a GtkEntry), forward
		 * the event to it before activating accelerator bindings too.
		 */
		if (gtk_window_propagate_key_event (GTK_WINDOW (window), event)) {
			return TRUE;
		}
	}

	for (i = 0; i < G_N_ELEMENTS (extra_window_keybindings); i++) {
		if (extra_window_keybindings[i].keyval == event->keyval) {
			const GList *action_groups;
			GtkAction *action;

			action = NULL;

			action_groups = gtk_ui_manager_get_action_groups (window->details->ui_manager);
			while (action_groups != NULL && action == NULL) {
				action = gtk_action_group_get_action (action_groups->data, extra_window_keybindings[i].action);
				action_groups = action_groups->next;
			}

			g_assert (action != NULL);
			if (gtk_action_is_sensitive (action)) {
				gtk_action_activate (action);
				return TRUE;
			}

			break;
		}
	}

    /* An alt key press by itself will always hide the menu if it's visible.  We set a flag
     * to skip the subsequent release, otherwise we'll show the menu again.
     *
     * When alt is pressed and the menu is NOT visible, we flag that on release we'll show the
     * menu.  If any other keys are pressed between alt being pressed and released, we clear that
     * flag, because it was more than likely part of some other shortcut, and otherwise, depending
     * on the order the keys are released, if the alt key is last to be released, we don't want to
     * show the menu, as that was not the original intent.
     */

    if (is_alt_key_event (event)) {
        if (gtk_widget_get_visible (window->details->menubar)) {
            toggle_menubar (window, MENU_HIDE);
            window->details->menu_skip_release = TRUE;
        } else {
            window->details->menu_show_queued = TRUE;
        }
    } else {
        window->details->menu_show_queued = FALSE;
    }

	return GTK_WIDGET_CLASS (kraken_window_parent_class)->key_press_event (widget, event);
}

static gboolean
kraken_window_key_release_event (GtkWidget *widget,
                             GdkEventKey *event)
{
    KrakenWindow *window = KRAKEN_WINDOW (widget);

    /* Conditions to show the menu via the alt key is that it must have been pressed and
     * released without any other key events in between, and we must not have hidden the
     * menu on the alt key press event.  Show we check both flags here, for opposing states.
     */

    if (is_alt_key_event (event)) {
        if (!window->details->menu_skip_release && window->details->menu_show_queued) {
            toggle_menubar (window, MENU_SHOW);
        }
    }

    window->details->menu_skip_release = FALSE;
    window->details->menu_show_queued = FALSE;

    return GTK_WIDGET_CLASS (kraken_window_parent_class)->key_release_event (widget, event);
}

/*
 * Main API
 */

static void
sync_view_type_callback (KrakenFile *file,
                         gpointer callback_data)
{
    KrakenWindow *window;
    KrakenWindowSlot *slot;

    slot = callback_data;
    window = kraken_window_slot_get_window (slot);

    if (slot == kraken_window_get_active_slot (window)) {
        const gchar *view_id;

        if (slot->content_view == NULL) {
            return;
        }

        view_id = kraken_window_slot_get_content_view_id (slot);

        toolbar_set_view_button (action_for_view_id (view_id), window);
        menu_set_view_selection (action_for_view_id (view_id), window);
    }
}

static void
cancel_sync_view_type_callback (KrakenWindowSlot *slot)
{
	kraken_file_cancel_call_when_ready (slot->viewed_file,
					      sync_view_type_callback,
					      slot);
}

void
kraken_window_sync_view_type (KrakenWindow *window)
{
    KrakenWindowSlot *slot;
    KrakenFileAttributes attributes;

    g_return_if_fail (KRAKEN_IS_WINDOW (window));

    attributes = kraken_mime_actions_get_required_file_attributes ();

    slot = kraken_window_get_active_slot (window);

    cancel_sync_view_type_callback (slot);
    kraken_file_call_when_ready (slot->viewed_file,
                               attributes,
                               sync_view_type_callback,
                               slot);
}

void
kraken_window_sync_menu_bar (KrakenWindow *window)
{
    GtkWidget *menu = window->details->menubar;

    if (g_settings_get_boolean (kraken_window_state, KRAKEN_WINDOW_STATE_START_WITH_MENU_BAR) &&
                                !window->details->disable_chrome) {
        gtk_widget_show (menu);
    } else {
        gtk_widget_hide (menu);
    }
}

void
kraken_window_sync_title (KrakenWindow *window,
			    KrakenWindowSlot *slot)
{
	KrakenWindowPane *pane;
	KrakenNotebook *notebook;
	char *full_title;
	char *window_title;

	if (KRAKEN_WINDOW_CLASS (G_OBJECT_GET_CLASS (window))->sync_title != NULL) {
		KRAKEN_WINDOW_CLASS (G_OBJECT_GET_CLASS (window))->sync_title (window, slot);

		return;
	}

	if (slot == kraken_window_get_active_slot (window)) {
		/* if spatial mode is default, we keep "File Browser" in the window title
		 * to recognize browser windows. Otherwise, we default to the directory name.
		 */
		if (!g_settings_get_boolean (kraken_preferences, KRAKEN_PREFERENCES_ALWAYS_USE_BROWSER)) {
			full_title = g_strdup_printf (_("%s - File Browser"), slot->title);
			window_title = eel_str_middle_truncate (full_title, MAX_TITLE_LENGTH);
			g_free (full_title);
		} else {
			window_title = eel_str_middle_truncate (slot->title, MAX_TITLE_LENGTH);
		}

		gtk_window_set_title (GTK_WINDOW (window), window_title);
		g_free (window_title);
	}

	pane = slot->pane;
	notebook = KRAKEN_NOTEBOOK (pane->notebook);
	kraken_notebook_sync_tab_label (notebook, slot);
}

void
kraken_window_sync_zoom_widgets (KrakenWindow *window)
{
	KrakenWindowSlot *slot;
	KrakenView *view;
	GtkActionGroup *action_group;
	GtkAction *action;
	gboolean supports_zooming;
	gboolean can_zoom, can_zoom_in, can_zoom_out;
	KrakenZoomLevel zoom_level;

	slot = kraken_window_get_active_slot (window);
	view = slot->content_view;

	if (view != NULL) {
		supports_zooming = kraken_view_supports_zooming (view);
		zoom_level = kraken_view_get_zoom_level (view);
		can_zoom = supports_zooming &&
			   zoom_level >= KRAKEN_ZOOM_LEVEL_SMALLEST &&
			   zoom_level <= KRAKEN_ZOOM_LEVEL_LARGEST;
		can_zoom_in = can_zoom && kraken_view_can_zoom_in (view);
		can_zoom_out = can_zoom && kraken_view_can_zoom_out (view);
	} else {
		supports_zooming = FALSE;
		can_zoom = FALSE;
		can_zoom_in = FALSE;
		can_zoom_out = FALSE;
	}

	action_group = kraken_window_get_main_action_group (window);

	action = gtk_action_group_get_action (action_group,
					      KRAKEN_ACTION_ZOOM_IN);
	gtk_action_set_visible (action, supports_zooming);
	gtk_action_set_sensitive (action, can_zoom_in);

	action = gtk_action_group_get_action (action_group,
					      KRAKEN_ACTION_ZOOM_OUT);
	gtk_action_set_visible (action, supports_zooming);
	gtk_action_set_sensitive (action, can_zoom_out);

	action = gtk_action_group_get_action (action_group,
					      KRAKEN_ACTION_ZOOM_NORMAL);
	gtk_action_set_visible (action, supports_zooming);
	gtk_action_set_sensitive (action, can_zoom);

    kraken_status_bar_sync_zoom_widgets (KRAKEN_STATUS_BAR (window->details->kraken_status_bar));
}

void
kraken_window_sync_bookmark_action (KrakenWindow *window)
{
    KrakenWindowSlot *slot;
    GFile *location;
    GtkAction *action;
    gchar *uri;
    slot = kraken_window_get_active_slot (window);
    location = kraken_window_slot_get_location (slot);

    if (!location) {
        return;
    }

    uri = g_file_get_uri (location);

    action = gtk_action_group_get_action (kraken_window_get_main_action_group (window),
                                          KRAKEN_ACTION_ADD_BOOKMARK);

    gtk_action_set_sensitive (action, !eel_uri_is_search (uri));

    g_free (uri);
    g_object_unref (location);
}

void
sync_thumbnail_action_callback (KrakenFile *file,
                       gpointer callback_data)
{
    KrakenWindow *window;
    KrakenWindowSlot *slot;

    slot = callback_data;
    window = kraken_window_slot_get_window (slot);

    if (slot == kraken_window_get_active_slot (window)) {
        KrakenWindowPane *pane;
        gboolean show_thumbnails;

        pane = kraken_window_get_active_pane(window);
        show_thumbnails = kraken_file_should_show_thumbnail (file);

        toolbar_set_show_thumbnails_button (show_thumbnails, pane);
        menu_set_show_thumbnails_action (show_thumbnails, window);
    }
}

static void
cancel_sync_show_thumbnail_callback (KrakenWindowSlot *slot)
{
	kraken_file_cancel_call_when_ready (slot->viewed_file,
					      sync_thumbnail_action_callback,
					      slot);
}

void
kraken_window_sync_thumbnail_action (KrakenWindow *window)
{
    KrakenWindowSlot *slot;
    KrakenFileAttributes attributes;

    g_return_if_fail (KRAKEN_IS_WINDOW (window));

    attributes = kraken_mime_actions_get_required_file_attributes ();

    slot = kraken_window_get_active_slot (window);

    cancel_sync_show_thumbnail_callback (slot);
    kraken_file_call_when_ready (slot->viewed_file,
                               attributes,
                               sync_thumbnail_action_callback,
                               slot);
}

void
kraken_window_sync_create_folder_button (KrakenWindow *window)
{
    KrakenWindowSlot *slot;
    gboolean allow;

    slot = kraken_window_get_active_slot (window);

    allow = kraken_file_can_write (slot->viewed_file) &&
            !kraken_file_is_in_favorites (slot->viewed_file) &&
            !kraken_file_is_in_trash (slot->viewed_file);

    toolbar_set_create_folder_button (allow, slot->pane);
}

static void
zoom_level_changed_callback (KrakenView *view,
                             KrakenWindow *window)
{
	g_assert (KRAKEN_IS_WINDOW (window));

	/* This is called each time the component in
	 * the active slot successfully completed
	 * a zooming operation.
	 */
	kraken_window_sync_zoom_widgets (window);
}


/* These are called
 *   A) when switching the view within the active slot
 *   B) when switching the active slot
 *   C) when closing the active slot (disconnect)
*/
void
kraken_window_connect_content_view (KrakenWindow *window,
				      KrakenView *view)
{
	KrakenWindowSlot *slot;

	g_assert (KRAKEN_IS_WINDOW (window));
	g_assert (KRAKEN_IS_VIEW (view));

	slot = kraken_window_get_slot_for_view (window, view);

	if (slot != kraken_window_get_active_slot (window)) {
		return;
	}

	g_signal_connect (view, "zoom-level-changed",
			  G_CALLBACK (zoom_level_changed_callback),
			  window);

    /* Update displayed the selected view type in the toolbar and menu. */
    if (slot->pending_location == NULL) {
        kraken_window_sync_view_type (window);
    }

	kraken_view_grab_focus (view);
}

void
kraken_window_disconnect_content_view (KrakenWindow *window,
					 KrakenView *view)
{
	KrakenWindowSlot *slot;

	g_assert (KRAKEN_IS_WINDOW (window));
	g_assert (KRAKEN_IS_VIEW (view));

	slot = kraken_window_get_slot_for_view (window, view);

	if (slot != kraken_window_get_active_slot (window)) {
		return;
	}

	g_signal_handlers_disconnect_by_func (view, G_CALLBACK (zoom_level_changed_callback), window);
}

/**
 * kraken_window_show:
 * @widget:	GtkWidget
 *
 * Call parent and then show/hide window items
 * base on user prefs.
 */
static void
kraken_window_show (GtkWidget *widget)
{
	KrakenWindow *window;

	window = KRAKEN_WINDOW (widget);

    window->details->sidebar_id = g_settings_get_string (kraken_window_state,
                                                         KRAKEN_WINDOW_STATE_SIDE_PANE_VIEW);

	if (g_settings_get_boolean (kraken_window_state, KRAKEN_WINDOW_STATE_START_WITH_SIDEBAR)) {
		kraken_window_show_sidebar (window);
	} else {
		kraken_window_hide_sidebar (window);
	}

	GTK_WIDGET_CLASS (kraken_window_parent_class)->show (widget);

	gtk_ui_manager_ensure_update (window->details->ui_manager);
}

GtkUIManager *
kraken_window_get_ui_manager (KrakenWindow *window)
{
	g_return_val_if_fail (KRAKEN_IS_WINDOW (window), NULL);

	return window->details->ui_manager;
}

GtkActionGroup *
kraken_window_get_main_action_group (KrakenWindow *window)
{
	g_return_val_if_fail (KRAKEN_IS_WINDOW (window), NULL);

	return window->details->main_action_group;
}

KrakenNavigationState *
kraken_window_get_navigation_state (KrakenWindow *window)
{
	g_return_val_if_fail (KRAKEN_IS_WINDOW (window), NULL);

	return window->details->nav_state;
}

KrakenWindowPane *
kraken_window_get_next_pane (KrakenWindow *window)
{
       KrakenWindowPane *next_pane;
       GList *node;

       /* return NULL if there is only one pane */
       if (!window->details->panes || !window->details->panes->next) {
	       return NULL;
       }

       /* get next pane in the (wrapped around) list */
       node = g_list_find (window->details->panes, window->details->active_pane);
       g_return_val_if_fail (node, NULL);
       if (node->next) {
	       next_pane = node->next->data;
       } else {
	       next_pane =  window->details->panes->data;
       }

       return next_pane;
}


void
kraken_window_slot_set_viewed_file (KrakenWindowSlot *slot,
				      KrakenFile *file)
{
	KrakenFileAttributes attributes;

	if (slot->viewed_file == file) {
		return;
	}

	kraken_file_ref (file);

	cancel_sync_view_type_callback (slot);
    cancel_sync_show_thumbnail_callback (slot);

	if (slot->viewed_file != NULL) {
		kraken_file_monitor_remove (slot->viewed_file,
					      slot);
	}

	if (file != NULL) {
		attributes =
			KRAKEN_FILE_ATTRIBUTE_INFO |
			KRAKEN_FILE_ATTRIBUTE_LINK_INFO;
		kraken_file_monitor_add (file, slot, attributes);
	}

	kraken_file_unref (slot->viewed_file);
	slot->viewed_file = file;
}

KrakenWindowSlot *
kraken_window_get_slot_for_view (KrakenWindow *window,
				   KrakenView *view)
{
	KrakenWindowSlot *slot;
	GList *l, *walk;

	for (walk = window->details->panes; walk; walk = walk->next) {
		KrakenWindowPane *pane = walk->data;

		for (l = pane->slots; l != NULL; l = l->next) {
			slot = l->data;
			if (slot->content_view == view ||
			    slot->new_content_view == view) {
				return slot;
			}
		}
	}

	return NULL;
}

KrakenWindowShowHiddenFilesMode
kraken_window_get_hidden_files_mode (KrakenWindow *window)
{
	return window->details->show_hidden_files_mode;
}

void
kraken_window_set_hidden_files_mode (KrakenWindow *window,
				       KrakenWindowShowHiddenFilesMode  mode)
{
	window->details->show_hidden_files_mode = mode;
    g_settings_set_boolean (kraken_preferences, KRAKEN_PREFERENCES_SHOW_HIDDEN_FILES,
                            mode == KRAKEN_WINDOW_SHOW_HIDDEN_FILES_ENABLE);
	g_signal_emit_by_name (window, "hidden_files_mode_changed");
}

KrakenWindowSlot *
kraken_window_get_active_slot (KrakenWindow *window)
{
	g_assert (KRAKEN_IS_WINDOW (window));

	if (window->details->active_pane == NULL) {
		return NULL;
	}

	return window->details->active_pane->active_slot;
}

KrakenWindowSlot *
kraken_window_get_extra_slot (KrakenWindow *window)
{
	KrakenWindowPane *extra_pane;
	GList *node;

	g_assert (KRAKEN_IS_WINDOW (window));


	/* return NULL if there is only one pane */
	if (window->details->panes == NULL ||
	    window->details->panes->next == NULL) {
		return NULL;
	}

	/* get next pane in the (wrapped around) list */
	node = g_list_find (window->details->panes,
			    window->details->active_pane);
	g_return_val_if_fail (node, FALSE);

	if (node->next) {
		extra_pane = node->next->data;
	}
	else {
		extra_pane =  window->details->panes->data;
	}

	return extra_pane->active_slot;
}

GList *
kraken_window_get_panes (KrakenWindow *window)
{
	g_assert (KRAKEN_IS_WINDOW (window));

	return window->details->panes;
}

static void
window_set_search_action_text (KrakenWindow *window,
			       gboolean setting)
{
	GtkAction *action;
	KrakenWindowPane *pane;
	GList *l;

	for (l = window->details->panes; l != NULL; l = l->next) {
		pane = l->data;
		action = gtk_action_group_get_action (pane->action_group,
						      KRAKEN_ACTION_SEARCH);

		gtk_action_set_is_important (action, setting);
	}
}

static void
center_pane_divider (GtkWidget  *paned,
                     GParamSpec *pspec,
                     gpointer    user_data)
{
    /* Make the paned think it's been manually resized, otherwise
     * things like the trash bar will force unwanted resizes */

    g_object_set (G_OBJECT (paned),
                  "position", gtk_widget_get_allocated_width (paned) / 2,
                  NULL);

    g_signal_handlers_disconnect_by_func (G_OBJECT (paned), center_pane_divider, NULL);
}

static KrakenWindowSlot *
create_extra_pane (KrakenWindow *window)
{
	KrakenWindowPane *pane;
	KrakenWindowSlot *slot;
	GtkPaned *paned;

	/* New pane */
	pane = kraken_window_pane_new (window);
	window->details->panes = g_list_append (window->details->panes, pane);

	paned = GTK_PANED (window->details->split_view_hpane);

    g_signal_connect_after (paned,
                            "notify::position",
                            G_CALLBACK(center_pane_divider),
                            NULL);

	if (gtk_paned_get_child1 (paned) == NULL) {
		gtk_paned_pack1 (paned, GTK_WIDGET (pane), TRUE, FALSE);
	} else {
		gtk_paned_pack2 (paned, GTK_WIDGET (pane), TRUE, FALSE);
	}

	/* Ensure the toolbar doesn't pop itself into existence (double toolbars suck.) */
	gtk_widget_hide (pane->tool_bar);

	/* slot */
	slot = kraken_window_pane_open_slot (KRAKEN_WINDOW_PANE (pane),
					       KRAKEN_WINDOW_OPEN_SLOT_APPEND);
	pane->active_slot = slot;

	return slot;
}

static void
kraken_window_reload (KrakenWindow *window)
{
	KrakenWindowSlot *active_slot;
	active_slot = kraken_window_get_active_slot (window);
	kraken_window_slot_queue_reload (active_slot, TRUE);
}

static gboolean
kraken_window_state_event (GtkWidget *widget,
			     GdkEventWindowState *event)
{
	if ((event->changed_mask & GDK_WINDOW_STATE_MAXIMIZED) && !kraken_window_is_desktop (KRAKEN_WINDOW (widget))) {
		g_settings_set_boolean (kraken_window_state, KRAKEN_WINDOW_STATE_MAXIMIZED,
					event->new_window_state & GDK_WINDOW_STATE_MAXIMIZED);
	}

	if (GTK_WIDGET_CLASS (kraken_window_parent_class)->window_state_event != NULL) {
		return GTK_WIDGET_CLASS (kraken_window_parent_class)->window_state_event (widget, event);
	}

	return FALSE;
}

static gboolean
kraken_window_delete_event (GtkWidget *widget,
			      GdkEventAny *event)
{
	kraken_window_close (KRAKEN_WINDOW (widget));
	return FALSE;
}

static gboolean
kraken_window_button_press_event (GtkWidget *widget,
				    GdkEventButton *event)
{
	KrakenWindow *window;
	gboolean handled;

	window = KRAKEN_WINDOW (widget);

	if (mouse_extra_buttons && (event->button == mouse_back_button)) {
		kraken_window_back_or_forward (window, TRUE, 0, 0);
		handled = TRUE;
	} else if (mouse_extra_buttons && (event->button == mouse_forward_button)) {
		kraken_window_back_or_forward (window, FALSE, 0, 0);
		handled = TRUE;
	} else if (GTK_WIDGET_CLASS (kraken_window_parent_class)->button_press_event) {
		handled = GTK_WIDGET_CLASS (kraken_window_parent_class)->button_press_event (widget, event);
	} else {
		handled = FALSE;
	}
	return handled;
}

static void
mouse_back_button_changed (gpointer callback_data)
{
	int new_back_button;

	new_back_button = g_settings_get_int (kraken_preferences, KRAKEN_PREFERENCES_MOUSE_BACK_BUTTON);

	/* Bounds checking */
	if (new_back_button < 6 || new_back_button > UPPER_MOUSE_LIMIT)
		return;

	mouse_back_button = new_back_button;
}

static void
mouse_forward_button_changed (gpointer callback_data)
{
	int new_forward_button;

	new_forward_button = g_settings_get_int (kraken_preferences, KRAKEN_PREFERENCES_MOUSE_FORWARD_BUTTON);

	/* Bounds checking */
	if (new_forward_button < 6 || new_forward_button > UPPER_MOUSE_LIMIT)
		return;

	mouse_forward_button = new_forward_button;
}

static void
use_extra_mouse_buttons_changed (gpointer callback_data)
{
	mouse_extra_buttons = g_settings_get_boolean (kraken_preferences, KRAKEN_PREFERENCES_MOUSE_USE_EXTRA_BUTTONS);
}


/*
 * Main API
 */

static void
kraken_window_init (KrakenWindow *window)
{
    GtkWindowGroup *window_group;

	window->details = G_TYPE_INSTANCE_GET_PRIVATE (window, KRAKEN_TYPE_WINDOW, KrakenWindowDetails);

	window->details->panes = NULL;
	window->details->active_pane = NULL;

    gboolean show_hidden = g_settings_get_boolean (kraken_preferences, KRAKEN_PREFERENCES_SHOW_HIDDEN_FILES);

    window->details->show_hidden_files_mode = show_hidden ? KRAKEN_WINDOW_SHOW_HIDDEN_FILES_ENABLE :
                                                            KRAKEN_WINDOW_SHOW_HIDDEN_FILES_DISABLE;

    window->details->show_sidebar = g_settings_get_boolean (kraken_window_state,
                                                            KRAKEN_WINDOW_STATE_START_WITH_SIDEBAR);

    window->details->menu_skip_release = FALSE;
    window->details->menu_show_queued = FALSE;

    window->details->ignore_meta_view_id = NULL;
    window->details->ignore_meta_zoom_level = -1;
    window->details->ignore_meta_visible_columns = NULL;
    window->details->ignore_meta_column_order = NULL;
    window->details->ignore_meta_sort_column = NULL;
    window->details->ignore_meta_sort_direction = SORT_NULL;

	/* This makes it possible for GTK+ themes to apply styling that is specific to Kraken
	 * without affecting other GTK+ applications.
	 */
	gtk_style_context_add_class (gtk_widget_get_style_context (GTK_WIDGET (window)), "kraken-window");

	window_group = gtk_window_group_new ();
	gtk_window_group_add_window (window_group, GTK_WINDOW (window));
	g_object_unref (window_group);

	/* Set initial window title */
	gtk_window_set_title (GTK_WINDOW (window), _("Kraken"));

    g_signal_connect_swapped (kraken_preferences,
				  "changed::" KRAKEN_PREFERENCES_SHOW_IMAGE_FILE_THUMBNAILS,
				  G_CALLBACK(kraken_window_sync_thumbnail_action),
				  window);
    g_signal_connect_swapped (kraken_preferences,
				  "changed::" KRAKEN_PREFERENCES_INHERIT_SHOW_THUMBNAILS,
				  G_CALLBACK(kraken_window_sync_thumbnail_action),
				  window);
}

static KrakenIconInfo *
real_get_icon (KrakenWindow *window,
               KrakenWindowSlot *slot)
{
        return kraken_file_get_icon (slot->viewed_file, 48, 0,
                       gtk_widget_get_scale_factor (GTK_WIDGET (window)),
				       KRAKEN_FILE_ICON_FLAGS_IGNORE_VISITING |
				       KRAKEN_FILE_ICON_FLAGS_USE_MOUNT_ICON);
}

static void
real_window_close (KrakenWindow *window)
{
	g_return_if_fail (KRAKEN_IS_WINDOW (window));

	kraken_window_save_geometry (window);

	gtk_widget_destroy (GTK_WIDGET (window));
}

static void
kraken_window_class_init (KrakenWindowClass *class)
{
	GtkBindingSet *binding_set;
	GObjectClass *oclass = G_OBJECT_CLASS (class);
	GtkWidgetClass *wclass = GTK_WIDGET_CLASS (class);

	oclass->finalize = kraken_window_finalize;
	oclass->constructed = kraken_window_constructed;
	oclass->get_property = kraken_window_get_property;
	oclass->set_property = kraken_window_set_property;

	wclass->destroy = kraken_window_destroy;
	wclass->show = kraken_window_show;
	wclass->realize = kraken_window_realize;
	wclass->key_press_event = kraken_window_key_press_event;
    wclass->key_release_event = kraken_window_key_release_event;
	wclass->window_state_event = kraken_window_state_event;
	wclass->button_press_event = kraken_window_button_press_event;
	wclass->delete_event = kraken_window_delete_event;

	class->get_icon = real_get_icon;
	class->close = real_window_close;

	properties[PROP_DISABLE_CHROME] =
		g_param_spec_boolean ("disable-chrome",
				      "Disable chrome",
				      "Disable window chrome, for the desktop",
				      FALSE,
				      G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
				      G_PARAM_STATIC_STRINGS);

    properties[PROP_SIDEBAR_VIEW_TYPE] =
        g_param_spec_string ("sidebar-view-id",
                      "Sidebar view type",
                      "Sidebar view type",
                      NULL,
                      G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

    properties[PROP_SHOW_SIDEBAR] =
        g_param_spec_boolean ("show-sidebar",
                              "Show the sidebar",
                              "Show the sidebar",
                              FALSE,
                              G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

	signals[GO_UP] =
		g_signal_new ("go-up",
			      G_TYPE_FROM_CLASS (class),
			      G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
			      G_STRUCT_OFFSET (KrakenWindowClass, go_up),
			      NULL, NULL,
			      g_cclosure_marshal_generic,
			      G_TYPE_NONE, 0);
	signals[RELOAD] =
		g_signal_new ("reload",
			      G_TYPE_FROM_CLASS (class),
			      G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
			      G_STRUCT_OFFSET (KrakenWindowClass, reload),
			      NULL, NULL,
			      g_cclosure_marshal_VOID__VOID,
			      G_TYPE_NONE, 0);
	signals[PROMPT_FOR_LOCATION] =
		g_signal_new ("prompt-for-location",
			      G_TYPE_FROM_CLASS (class),
			      G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
			      G_STRUCT_OFFSET (KrakenWindowClass, prompt_for_location),
			      NULL, NULL,
			      g_cclosure_marshal_VOID__STRING,
			      G_TYPE_NONE, 1, G_TYPE_STRING);
	signals[HIDDEN_FILES_MODE_CHANGED] =
		g_signal_new ("hidden_files_mode_changed",
			      G_TYPE_FROM_CLASS (class),
			      G_SIGNAL_RUN_LAST,
			      0,
			      NULL, NULL,
			      g_cclosure_marshal_VOID__VOID,
			      G_TYPE_NONE, 0);
	signals[LOADING_URI] =
		g_signal_new ("loading_uri",
			      G_TYPE_FROM_CLASS (class),
			      G_SIGNAL_RUN_LAST,
			      0,
			      NULL, NULL,
			      g_cclosure_marshal_VOID__STRING,
			      G_TYPE_NONE, 1,
			      G_TYPE_STRING);
	signals[SLOT_ADDED] =
		g_signal_new ("slot-added",
			      G_TYPE_FROM_CLASS (class),
			      G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
			      0,
			      NULL, NULL,
			      g_cclosure_marshal_VOID__OBJECT,
			      G_TYPE_NONE, 1, KRAKEN_TYPE_WINDOW_SLOT);
	signals[SLOT_REMOVED] =
		g_signal_new ("slot-removed",
			      G_TYPE_FROM_CLASS (class),
			      G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
			      0,
			      NULL, NULL,
			      g_cclosure_marshal_VOID__OBJECT,
			      G_TYPE_NONE, 1, KRAKEN_TYPE_WINDOW_SLOT);

	binding_set = gtk_binding_set_by_class (class);
	gtk_binding_entry_add_signal (binding_set, GDK_KEY_BackSpace, 0,
				      "go-up", 0);
	gtk_binding_entry_add_signal (binding_set, GDK_KEY_F5, 0,
				      "reload", 0);
	gtk_binding_entry_add_signal (binding_set, GDK_KEY_slash, 0,
				      "prompt-for-location", 1,
				      G_TYPE_STRING, "/");
	gtk_binding_entry_add_signal (binding_set, GDK_KEY_KP_Divide, 0,
				      "prompt-for-location", 1,
				      G_TYPE_STRING, "/");
	gtk_binding_entry_add_signal (binding_set, GDK_KEY_asciitilde, 0,
				      "prompt-for-location", 1,
				      G_TYPE_STRING, "~");

	class->reload = kraken_window_reload;
	class->go_up = kraken_window_go_up_signal;
	class->prompt_for_location = kraken_window_prompt_for_location;

	g_signal_connect_swapped (kraken_preferences,
				  "changed::" KRAKEN_PREFERENCES_MOUSE_BACK_BUTTON,
				  G_CALLBACK(mouse_back_button_changed),
				  NULL);

	g_signal_connect_swapped (kraken_preferences,
				  "changed::" KRAKEN_PREFERENCES_MOUSE_FORWARD_BUTTON,
				  G_CALLBACK(mouse_forward_button_changed),
				  NULL);

	g_signal_connect_swapped (kraken_preferences,
				  "changed::" KRAKEN_PREFERENCES_MOUSE_USE_EXTRA_BUTTONS,
				  G_CALLBACK(use_extra_mouse_buttons_changed),
				  NULL);

	g_object_class_install_properties (oclass, NUM_PROPERTIES, properties);
	g_type_class_add_private (oclass, sizeof (KrakenWindowDetails));
}

KrakenWindow *
kraken_window_new (GtkApplication *application,
                 GdkScreen *screen)
{
	return g_object_new (KRAKEN_TYPE_WINDOW,
			     "application", application,
			     "screen", screen,
			     NULL);
}

void
kraken_window_split_view_on (KrakenWindow *window)
{
	KrakenWindowSlot *slot, *old_active_slot;
	GFile *location;

	old_active_slot = kraken_window_get_active_slot (window);
	slot = create_extra_pane (window);

    location = window->details->secondary_pane_last_location;

	if (location == NULL && old_active_slot != NULL) {
		location = kraken_window_slot_get_location (old_active_slot);
		if (location != NULL) {
			if (g_file_has_uri_scheme (location, "x-kraken-search")) {
				g_object_unref (location);
				location = NULL;
			}
		}
	}
	if (location == NULL) {
		location = g_file_new_for_path (g_get_home_dir ());
	}

	kraken_window_slot_open_location (slot, location, 0);
	g_object_unref (location);

	window_set_search_action_text (window, FALSE);
}

void
kraken_window_split_view_off (KrakenWindow *window)
{
	KrakenWindowPane *pane, *active_pane;
	GList *l, *next;

	active_pane = kraken_window_get_active_pane (window);

	/* delete all panes except the first (main) pane */
	for (l = window->details->panes; l != NULL; l = next) {
		next = l->next;
		pane = l->data;
		if (pane != active_pane) {
            g_clear_object (&window->details->secondary_pane_last_location);
            window->details->secondary_pane_last_location = kraken_window_slot_get_location (pane->active_slot);
			kraken_window_close_pane (window, pane);
		}
	}

    /* Reset split view pane's position so the position can be
     * caught again later */
    g_object_set (G_OBJECT (window->details->split_view_hpane),
                  "position", 0,
                  "position-set", FALSE,
                  NULL);

	kraken_window_set_active_pane (window, active_pane);
	kraken_navigation_state_set_master (window->details->nav_state,
					      active_pane->action_group);

	kraken_window_update_show_hide_ui_elements (window);
}

gboolean
kraken_window_split_view_showing (KrakenWindow *window)
{
	return g_list_length (KRAKEN_WINDOW (window)->details->panes) > 1;
}

void
kraken_window_clear_secondary_pane_location (KrakenWindow *window)
{
    g_return_if_fail (KRAKEN_IS_WINDOW (window));
    g_clear_object (&window->details->secondary_pane_last_location);
}

void
kraken_window_set_sidebar_id (KrakenWindow *window,
                            const gchar *id)
{
    if (g_strcmp0 (id, window->details->sidebar_id) != 0) {

        g_settings_set_string (kraken_window_state,
                               KRAKEN_WINDOW_STATE_SIDE_PANE_VIEW,
                               id);

        g_free (window->details->sidebar_id);

        window->details->sidebar_id = g_strdup (id);

        g_object_notify_by_pspec (G_OBJECT (window), properties[PROP_SIDEBAR_VIEW_TYPE]);
    }
}

const gchar *
kraken_window_get_sidebar_id (KrakenWindow *window)
{
    return window->details->sidebar_id;
}

void
kraken_window_set_show_sidebar (KrakenWindow *window,
                              gboolean show)
{
    if (!KRAKEN_IS_DESKTOP_WINDOW (window)) {
        window->details->show_sidebar = show;

        g_settings_set_boolean (kraken_window_state, KRAKEN_WINDOW_STATE_START_WITH_SIDEBAR, show);

        g_object_notify_by_pspec (G_OBJECT (window), properties[PROP_SHOW_SIDEBAR]);
    }
}

gboolean
kraken_window_get_show_sidebar (KrakenWindow *window)
{
    return window->details->show_sidebar;
}

const gchar *
kraken_window_get_ignore_meta_view_id (KrakenWindow *window)
{
    return window->details->ignore_meta_view_id;
}

void
kraken_window_set_ignore_meta_view_id (KrakenWindow *window, const gchar *id)
{
    if (id != NULL) {
        gchar *old_id = window->details->ignore_meta_view_id;
        if (g_strcmp0 (old_id, id) != 0) {
            kraken_window_set_ignore_meta_zoom_level (window, -1);
        }
        window->details->ignore_meta_view_id = g_strdup (id);
        g_free (old_id);
    }
}

gint
kraken_window_get_ignore_meta_zoom_level (KrakenWindow *window)
{
    return window->details->ignore_meta_zoom_level;
}

void
kraken_window_set_ignore_meta_zoom_level (KrakenWindow *window, gint level)
{
    window->details->ignore_meta_zoom_level = level;
}

GList *
kraken_window_get_ignore_meta_visible_columns (KrakenWindow *window)
{
    return g_list_copy_deep (window->details->ignore_meta_visible_columns, (GCopyFunc) g_strdup, NULL);
}

void
kraken_window_set_ignore_meta_visible_columns (KrakenWindow *window, GList *list)
{
    GList *old = window->details->ignore_meta_visible_columns;
    window->details->ignore_meta_visible_columns = list != NULL ? g_list_copy_deep (list, (GCopyFunc) g_strdup, NULL) :
                                                                  NULL;
    if (old != NULL)
        g_list_free_full (old, g_free);
}

GList *
kraken_window_get_ignore_meta_column_order (KrakenWindow *window)
{
    return g_list_copy_deep (window->details->ignore_meta_column_order, (GCopyFunc) g_strdup, NULL);
}

void
kraken_window_set_ignore_meta_column_order (KrakenWindow *window, GList *list)
{
    GList *old = window->details->ignore_meta_column_order;
    window->details->ignore_meta_column_order = list != NULL ? g_list_copy_deep (list, (GCopyFunc) g_strdup, NULL) :
                                                               NULL;
    if (old != NULL)
        g_list_free_full (old, g_free);
}

const gchar *
kraken_window_get_ignore_meta_sort_column (KrakenWindow *window)
{
    return window->details->ignore_meta_sort_column;
}

void
kraken_window_set_ignore_meta_sort_column (KrakenWindow *window, const gchar *column)
{
    if (column != NULL) {
        gchar *old_column = window->details->ignore_meta_sort_column;
        window->details->ignore_meta_sort_column = g_strdup (column);
        g_free (old_column);
    }
}

gint
kraken_window_get_ignore_meta_sort_direction (KrakenWindow *window)
{
    return window->details->ignore_meta_sort_direction;
}

void
kraken_window_set_ignore_meta_sort_direction (KrakenWindow *window, gint direction)
{
    window->details->ignore_meta_sort_direction = direction;
}

KrakenWindowOpenFlags
kraken_event_get_window_open_flags (void)
{
	KrakenWindowOpenFlags flags = 0;
	GdkEvent *event;

	event = gtk_get_current_event ();

	if (event == NULL) {
		return flags;
	}

	if ((event->type == GDK_BUTTON_PRESS || event->type == GDK_BUTTON_RELEASE) &&
	    (event->button.button == 2)) {
		flags |= KRAKEN_WINDOW_OPEN_FLAG_NEW_TAB;
	}

	gdk_event_free (event);

	return flags;
}
