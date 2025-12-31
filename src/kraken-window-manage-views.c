/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/*
 *  Kraken
 *
 *  Copyright (C) 1999, 2000 Red Hat, Inc.
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
 *           John Sullivan <sullivan@eazel.com>
 *           Darin Adler <darin@bentspoon.com>
 */

#include <config.h>
#include "kraken-window-manage-views.h"

#include "kraken-actions.h"
#include "kraken-application.h"
#include "kraken-floating-bar.h"
#include "kraken-location-bar.h"
#include "kraken-pathbar.h"
#include "kraken-window-private.h"
#include "kraken-window-slot.h"
#include "kraken-trash-bar.h"
#include "kraken-view-factory.h"
#include "kraken-x-content-bar.h"
#include "kraken-interesting-folder-bar.h"
#include "kraken-thumbnail-problem-bar.h"
#include <eel/eel-accessibility.h>
#include <eel/eel-debug.h>
#include <eel/eel-glib-extensions.h>
#include <eel/eel-stock-dialogs.h>
#include <eel/eel-string.h>
#include <eel/eel-vfs-extensions.h>
#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <glib/gi18n.h>
#include <libkraken-extension/kraken-location-widget-provider.h>
#include <libkraken-private/kraken-desktop-directory.h>
#include <libkraken-private/kraken-file-attributes.h>
#include <libkraken-private/kraken-file-utilities.h>
#include <libkraken-private/kraken-file.h>
#include <libkraken-private/kraken-global-preferences.h>
#include <libkraken-private/kraken-metadata.h>
#include <libkraken-private/kraken-module.h>
#include <libkraken-private/kraken-monitor.h>
#include <libkraken-private/kraken-search-directory.h>

#define DEBUG_FLAG KRAKEN_DEBUG_WINDOW
#include <libkraken-private/kraken-debug.h>

/* FIXME bugzilla.gnome.org 41243:
 * We should use inheritance instead of these special cases
 * for the desktop window.
 */
#include "kraken-desktop-window.h"

/* This number controls a maximum character count for a URL that is
 * displayed as part of a dialog. It's fairly arbitrary -- big enough
 * to allow most "normal" URIs to display in full, but small enough to
 * prevent the dialog from getting insanely wide.
 */
#define MAX_URI_IN_DIALOG_LENGTH 60

static void begin_location_change                     (KrakenWindowSlot         *slot,
                                                       GFile                      *location,
                                                       GFile                      *previous_location,
                                                       GList                      *new_selection,
                                                       KrakenLocationChangeType  type,
                                                       guint                       distance,
                                                       const char                 *scroll_pos,
                                                       gboolean                    mount,
                                                       KrakenWindowGoToCallback      callback,
                                                       gpointer                    user_data);
static void free_location_change                      (KrakenWindowSlot         *slot);
static void end_location_change                       (KrakenWindowSlot         *slot);
static void cancel_location_change                    (KrakenWindowSlot         *slot);
static void got_file_info_for_view_selection_callback (KrakenFile               *file,
						       gpointer                    callback_data);
static void create_content_view                       (KrakenWindowSlot         *slot,
						       const char                 *view_id);
static void display_view_selection_failure            (KrakenWindow             *window,
						       KrakenFile               *file,
						       GFile                      *location,
						       GError                     *error);
static void load_new_location                         (KrakenWindowSlot         *slot,
						       GFile                      *location,
						       GList                      *selection,
						       gboolean                    tell_current_content_view,
						       gboolean                    tell_new_content_view);
static void location_has_really_changed               (KrakenWindowSlot         *slot);
static void update_for_new_location                   (KrakenWindowSlot         *slot);

/* set_displayed_location:
 */
static void
set_displayed_location (KrakenWindowSlot *slot, GFile *location)
{
        GFile *bookmark_location;
        gboolean recreate;

        if (slot->current_location_bookmark == NULL || location == NULL) {
                recreate = TRUE;
        } else {
                bookmark_location = kraken_bookmark_get_location (slot->current_location_bookmark);
                recreate = !g_file_equal (bookmark_location, location);
                g_object_unref (bookmark_location);
        }

    if (recreate) {
        /* We've changed locations, must recreate bookmark for current location. */
        g_clear_object (&slot->last_location_bookmark);

        slot->last_location_bookmark = slot->current_location_bookmark;
        slot->current_location_bookmark = (location == NULL) ?
                            NULL : kraken_bookmark_new (location, NULL, NULL, NULL);
    }
}

static void
check_bookmark_location_matches (KrakenBookmark *bookmark, GFile *location)
{
        GFile *bookmark_location;
        char *bookmark_uri, *uri;

	bookmark_location = kraken_bookmark_get_location (bookmark);
	if (!g_file_equal (location, bookmark_location)) {
		bookmark_uri = g_file_get_uri (bookmark_location);
		uri = g_file_get_uri (location);
		g_warning ("bookmark uri is %s, but expected %s", bookmark_uri, uri);
		g_free (uri);
		g_free (bookmark_uri);
	}
	g_object_unref (bookmark_location);
}

/* Debugging function used to verify that the last_location_bookmark
 * is in the state we expect when we're about to use it to update the
 * Back or Forward list.
 */
static void
check_last_bookmark_location_matches_slot (KrakenWindowSlot *slot)
{
	check_bookmark_location_matches (slot->last_location_bookmark,
					 slot->location);
}

static void
handle_go_back (KrakenWindowSlot *slot,
		GFile *location)
{
	guint i;
	GList *link;
	KrakenBookmark *bookmark;

	/* Going back. Move items from the back list to the forward list. */
	g_assert (g_list_length (slot->back_list) > slot->location_change_distance);
	check_bookmark_location_matches (KRAKEN_BOOKMARK (g_list_nth_data (slot->back_list,
									     slot->location_change_distance)),
					 location);
	g_assert (slot->location != NULL);

	/* Move current location to Forward list */

	check_last_bookmark_location_matches_slot (slot);

	/* Use the first bookmark in the history list rather than creating a new one. */
	slot->forward_list = g_list_prepend (slot->forward_list,
					     slot->last_location_bookmark);
	g_object_ref (slot->forward_list->data);

	/* Move extra links from Back to Forward list */
	for (i = 0; i < slot->location_change_distance; ++i) {
		bookmark = KRAKEN_BOOKMARK (slot->back_list->data);
		slot->back_list =
			g_list_remove (slot->back_list, bookmark);
		slot->forward_list =
			g_list_prepend (slot->forward_list, bookmark);
	}

	/* One bookmark falls out of back/forward lists and becomes viewed location */
	link = slot->back_list;
	slot->back_list = g_list_remove_link (slot->back_list, link);
	g_object_unref (link->data);
	g_list_free_1 (link);
}

static void
handle_go_forward (KrakenWindowSlot *slot,
		   GFile *location)
{
	guint i;
	GList *link;
	KrakenBookmark *bookmark;

	/* Going forward. Move items from the forward list to the back list. */
	g_assert (g_list_length (slot->forward_list) > slot->location_change_distance);
	check_bookmark_location_matches (KRAKEN_BOOKMARK (g_list_nth_data (slot->forward_list,
									     slot->location_change_distance)),
					 location);
	g_assert (slot->location != NULL);

	/* Move current location to Back list */
	check_last_bookmark_location_matches_slot (slot);

	/* Use the first bookmark in the history list rather than creating a new one. */
	slot->back_list = g_list_prepend (slot->back_list,
						     slot->last_location_bookmark);
	g_object_ref (slot->back_list->data);

	/* Move extra links from Forward to Back list */
	for (i = 0; i < slot->location_change_distance; ++i) {
		bookmark = KRAKEN_BOOKMARK (slot->forward_list->data);
		slot->forward_list =
			g_list_remove (slot->back_list, bookmark);
		slot->back_list =
			g_list_prepend (slot->forward_list, bookmark);
	}

	/* One bookmark falls out of back/forward lists and becomes viewed location */
	link = slot->forward_list;
	slot->forward_list = g_list_remove_link (slot->forward_list, link);
	g_object_unref (link->data);
	g_list_free_1 (link);
}

static void
handle_go_elsewhere (KrakenWindowSlot *slot,
		     GFile *location)
{
	/* Clobber the entire forward list, and move displayed location to back list */
	kraken_window_slot_clear_forward_list (slot);

	if (slot->location != NULL) {
		/* If we're returning to the same uri somehow, don't put this uri on back list.
		 * This also avoids a problem where set_displayed_location
		 * didn't update last_location_bookmark since the uri didn't change.
		 */
		if (!g_file_equal (slot->location, location)) {
			/* Store bookmark for current location in back list, unless there is no current location */
			check_last_bookmark_location_matches_slot (slot);
			/* Use the first bookmark in the history list rather than creating a new one. */
			slot->back_list = g_list_prepend (slot->back_list,
							  slot->last_location_bookmark);
			g_object_ref (slot->back_list->data);
		}
	}
}

static void
viewed_file_changed_callback (KrakenFile *file,
                              KrakenWindowSlot *slot)
{
        GFile *new_location;
	gboolean is_in_trash, was_in_trash;

        g_assert (KRAKEN_IS_FILE (file));
	g_assert (KRAKEN_IS_WINDOW_PANE (slot->pane));
	g_assert (file == slot->viewed_file);

        if (!kraken_file_is_not_yet_confirmed (file)) {
                slot->viewed_file_seen = TRUE;
        }

	was_in_trash = slot->viewed_file_in_trash;

	slot->viewed_file_in_trash = is_in_trash = kraken_file_is_in_trash (file);

    /* Close window if the file it's viewing has been deleted or moved to trash. */
    if (kraken_file_is_gone (file) || (is_in_trash && !was_in_trash)) {
        KrakenFile *parent;
        gboolean parent_is_desktop = FALSE;

        parent = kraken_file_get_parent (file);

        if (parent != NULL) {
            parent_is_desktop = kraken_file_is_desktop_directory (parent);
            kraken_file_unref (parent);
        }

        if (slot->back_list == NULL && parent_is_desktop) {
            end_location_change (slot);
            gtk_widget_destroy (GTK_WIDGET (slot->content_view));
            kraken_window_pane_close_slot (slot->pane, slot);
            return;
        }

        /* Don't close the window in the case where the
        * file was never seen in the first place.
        */
        if (slot->viewed_file_seen) {
            /* auto-show existing parent. */
            GFile *go_to_file, *parent, *location;

            /* Detecting a file is gone may happen in the
            * middle of a pending location change, we
            * need to cancel it before closing the window
            * or things break.
            */
            /* FIXME: It makes no sense that this call is
            * needed. When the window is destroyed, it
            * calls kraken_window_manage_views_destroy,
            * which calls free_location_change, which
            * should be sufficient. Also, if this was
            * really needed, wouldn't it be needed for
            * all other kraken_window_close callers?
            */
            end_location_change (slot);

            go_to_file = NULL;
            location =  kraken_file_get_location (file);
            parent = g_file_get_parent (location);
            g_object_unref (location);

            if (parent) {
                go_to_file = kraken_find_existing_uri_in_hierarchy (parent);
                g_object_unref (parent);
            }

            if (go_to_file != NULL) {
                /* the path bar URI will be set to go_to_uri immediately
                * in begin_location_change, but we don't want the
                * inexistant children to show up anymore */
                if (slot == slot->pane->active_slot) {
                    /* multiview-TODO also update KrakenWindowSlot
                    * [which as of writing doesn't save/store any path bar state]
                    */
                    kraken_path_bar_clear_buttons (KRAKEN_PATH_BAR (slot->pane->path_bar));
                }

                kraken_window_slot_open_location (slot, go_to_file, 0);
                g_object_unref (go_to_file);
            } else {
                kraken_window_slot_go_home (slot, FALSE);
            }
        }
    } else {
        new_location = kraken_file_get_location (file);

        /* If the file was renamed, update location and/or
         * title. */
        if (!g_file_equal (new_location, slot->location)) {
            g_object_unref (slot->location);
            slot->location = new_location;

            if (slot == slot->pane->active_slot) {
                kraken_window_pane_sync_location_widgets (slot->pane);
            }
        } else {
            /* TODO?
            *   why do we update title & icon at all in this case? */
            g_object_unref (new_location);
        }

        kraken_window_slot_update_title (slot);
        kraken_window_slot_update_icon (slot);
    }
}

static void
update_history (KrakenWindowSlot *slot,
                KrakenLocationChangeType type,
                GFile *new_location)
{
        switch (type) {
        case KRAKEN_LOCATION_CHANGE_STANDARD:
		handle_go_elsewhere (slot, new_location);
                return;
        case KRAKEN_LOCATION_CHANGE_RELOAD:
                /* for reload there is no work to do */
                return;
        case KRAKEN_LOCATION_CHANGE_BACK:
                handle_go_back (slot, new_location);
                return;
        case KRAKEN_LOCATION_CHANGE_FORWARD:
                handle_go_forward (slot, new_location);
                return;
        default:
            break;
        }
	g_return_if_fail (FALSE);
}

static void
cancel_viewed_file_changed_callback (KrakenWindowSlot *slot)
{
        KrakenFile *file;

        file = slot->viewed_file;
        if (file != NULL) {
                g_signal_handlers_disconnect_by_func (G_OBJECT (file),
                                                      G_CALLBACK (viewed_file_changed_callback),
						      slot);
                kraken_file_monitor_remove (file, &slot->viewed_file);
        }
}

static void
new_window_show_callback (GtkWidget *widget,
			  gpointer user_data){
	KrakenWindow *window;

	window = KRAKEN_WINDOW (user_data);
	kraken_window_close (window);

	g_signal_handlers_disconnect_by_func (widget,
					      G_CALLBACK (new_window_show_callback),
					      user_data);
}

void
kraken_window_slot_open_location_full (KrakenWindowSlot *slot,
					 GFile *location,
					 KrakenWindowOpenFlags flags,
					 GList *new_selection,
					 KrakenWindowGoToCallback callback,
					 gpointer user_data)
{
	KrakenWindow *window;
        KrakenWindow *target_window;
        KrakenWindowPane *pane;
        KrakenWindowSlot *target_slot;
	KrakenWindowOpenFlags slot_flags;
	GFile *old_location;
	char *old_uri, *new_uri;
	int new_slot_position;
	GList *l;
	gboolean use_same;
	gboolean is_desktop;
	KrakenApplication *app;

	window = kraken_window_slot_get_window (slot);

        target_window = NULL;
	target_slot = NULL;
	use_same = FALSE;

	/* this happens at startup */
	old_uri = kraken_window_slot_get_location_uri (slot);
	if (old_uri == NULL) {
		old_uri = g_strdup ("(none)");
		use_same = TRUE;
	}
	new_uri = g_file_get_uri (location);

	DEBUG ("Opening location, old: %s, new: %s", old_uri, new_uri);

	g_free (old_uri);
	g_free (new_uri);

	is_desktop = KRAKEN_IS_DESKTOP_WINDOW (window);

	if (is_desktop) {
		use_same = !kraken_desktop_window_loaded (KRAKEN_DESKTOP_WINDOW (window));

		/* if we're requested to open a new tab on the desktop, open a window
		 * instead.
		 */
		if (flags & KRAKEN_WINDOW_OPEN_FLAG_NEW_TAB) {
			flags ^= KRAKEN_WINDOW_OPEN_FLAG_NEW_TAB;
			flags |= KRAKEN_WINDOW_OPEN_FLAG_NEW_WINDOW;
		}
	} else {
		use_same |= g_settings_get_boolean (kraken_preferences, KRAKEN_PREFERENCES_ALWAYS_USE_BROWSER);
	}

	g_assert (!((flags & KRAKEN_WINDOW_OPEN_FLAG_NEW_WINDOW) != 0 &&
		    (flags & KRAKEN_WINDOW_OPEN_FLAG_NEW_TAB) != 0));

	/* and if the flags specify so, this is overridden */
	if ((flags & KRAKEN_WINDOW_OPEN_FLAG_SEARCH) != 0) {
		use_same = TRUE;
	}
	else if ((flags & KRAKEN_WINDOW_OPEN_FLAG_NEW_WINDOW) != 0) {
		use_same = FALSE;
	}

	/* now get/create the window */
	if (use_same) {
		target_window = window;
	} else {
		app = kraken_application_get_singleton ();
        target_window = kraken_application_create_window (app, gtk_window_get_screen (GTK_WINDOW (window)));
	}

    old_location = kraken_window_slot_get_location (slot);

    g_assert (target_window != NULL);

	/* if the flags say we want a new tab, open a slot in the current window */
	if ((flags & KRAKEN_WINDOW_OPEN_FLAG_NEW_TAB) != 0) {
		g_assert (target_window == window);

		slot_flags = 0;

		new_slot_position = g_settings_get_enum (kraken_preferences, KRAKEN_PREFERENCES_NEW_TAB_POSITION);
		if (new_slot_position == KRAKEN_NEW_TAB_POSITION_END) {
			slot_flags = KRAKEN_WINDOW_OPEN_SLOT_APPEND;
		}

		target_slot = kraken_window_pane_open_slot (kraken_window_get_active_pane (window),
							      slot_flags);
	}

	/* close the current window if the flags say so */
	if ((flags & KRAKEN_WINDOW_OPEN_FLAG_CLOSE_BEHIND) != 0) {
		if (!is_desktop) {
			if (gtk_widget_get_visible (GTK_WIDGET (target_window))) {
				kraken_window_close (window);
			} else {
				g_signal_connect_object (target_window,
							 "show",
							 G_CALLBACK (new_window_show_callback),
							 window,
							 G_CONNECT_AFTER);
			}
		}
	}

	if (target_slot == NULL) {
		if (target_window == window) {
			target_slot = slot;
		} else {
			target_slot = kraken_window_get_active_slot (target_window);
		}
	}

    if (target_window == window && target_slot == slot &&
        old_location && g_file_equal (old_location, location) &&
        !is_desktop) {

        if (callback != NULL) {
        	callback (window, NULL, user_data);
        }

        g_object_unref (old_location);
            return;
    }

    begin_location_change (target_slot,
                           location,
                           old_location,
                           new_selection,
                           KRAKEN_LOCATION_CHANGE_STANDARD,
                           0, NULL,
                           (flags & KRAKEN_WINDOW_OPEN_FLAG_MOUNT),
                           callback,
                           user_data);

    /* Additionally, load this in all slots that have no location, this means
    we load both panes in e.g. a newly opened dual pane window. */
    for (l = target_window->details->panes; l != NULL; l = l->next) {
        pane = l->data;
        slot = pane->active_slot;

        if (slot->location == NULL && slot->pending_location == NULL) {
            begin_location_change (slot,
                                   location,
                                   old_location,
                                   new_selection,
                                   KRAKEN_LOCATION_CHANGE_STANDARD,
                                   0, NULL,
                                   (flags & KRAKEN_WINDOW_OPEN_FLAG_MOUNT),
                                   NULL,
                                   NULL);
        }
    }

    g_clear_object (&old_location);
}

const char *
kraken_window_slot_get_content_view_id (KrakenWindowSlot *slot)
{
	if (slot->content_view == NULL) {
		return NULL;
	}
	return kraken_view_get_view_id (slot->content_view);
}

gboolean
kraken_window_slot_content_view_matches_iid (KrakenWindowSlot *slot,
					       const char *iid)
{
	if (slot->content_view == NULL) {
		return FALSE;
	}
	return g_strcmp0 (kraken_view_get_view_id (slot->content_view), iid) == 0;
}

static gboolean
report_callback (KrakenWindowSlot *slot,
		 GError *error)
{
	if (slot->open_callback != NULL) {
		slot->open_callback (kraken_window_slot_get_window (slot),
				     error, slot->open_callback_user_data);
		slot->open_callback = NULL;
		slot->open_callback_user_data = NULL;

		return TRUE;
	}

	return FALSE;
}

/*
 * begin_location_change
 *
 * Change a window slot's location.
 * @window: The KrakenWindow whose location should be changed.
 * @location: A url specifying the location to load
 * @previous_location: The url that was previously shown in the window that initialized the change, if any
 * @new_selection: The initial selection to present after loading the location
 * @type: Which type of location change is this? Standard, back, forward, or reload?
 * @distance: If type is back or forward, the index into the back or forward chain. If
 * type is standard or reload, this is ignored, and must be 0.
 * @scroll_pos: The file to scroll to when the location is loaded.
 * @mount: is a mount (always force a reload).
 * @callback: function to be called when the location is changed.
 * @user_data: data for @callback.
 *
 * This is the core function for changing the location of a window. Every change to the
 * location begins here.
 */
static void
begin_location_change (KrakenWindowSlot        *slot,
                       GFile                 *location,
                       GFile                 *previous_location,
                       GList                 *new_selection,
                       KrakenLocationChangeType type,
                       guint                  distance,
                       const char            *scroll_pos,
                       gboolean               mount,
                       KrakenWindowGoToCallback callback,
                       gpointer               user_data)
{
        KrakenDirectory *directory;
        KrakenFile *file;
	gboolean force_reload;
        char *current_pos;
	GFile *from_folder, *parent;
	GList *parent_selection = NULL;

	g_assert (slot != NULL);
        g_assert (location != NULL);
        g_assert (type == KRAKEN_LOCATION_CHANGE_BACK
                  || type == KRAKEN_LOCATION_CHANGE_FORWARD
                  || distance == 0);

	/* If there is no new selection and the new location is
	 * a (grand)parent of the old location then we automatically
	 * select the folder the previous location was in */
	if (new_selection == NULL && previous_location != NULL &&
	    g_file_has_prefix (previous_location, location)) {
		from_folder = g_object_ref (previous_location);
		parent = g_file_get_parent (from_folder);
		while (parent != NULL && !g_file_equal (parent, location)) {
			g_object_unref (from_folder);
			from_folder = parent;
			parent = g_file_get_parent (from_folder);
		}

		if (parent != NULL) {
			new_selection = parent_selection =
				g_list_prepend (NULL, kraken_file_get (from_folder));
			g_object_unref (parent);
		}

		g_object_unref (from_folder);
	}

	end_location_change (slot);

	kraken_window_slot_set_allow_stop (slot, TRUE);
	kraken_window_slot_set_status (slot, " ", NULL, FALSE);

	g_assert (slot->pending_location == NULL);
	g_assert (slot->pending_selection == NULL);

	slot->pending_location = g_object_ref (location);
        slot->location_change_type = type;
        slot->location_change_distance = distance;
	slot->tried_mount = FALSE;
	slot->pending_selection = eel_g_object_list_copy (new_selection);

	slot->pending_scroll_to = g_strdup (scroll_pos);

	slot->open_callback = callback;
	slot->open_callback_user_data = user_data;

    directory = kraken_directory_get (location);

	/* The code to force a reload is here because if we do it
	 * after determining an initial view (in the components), then
	 * we end up fetching things twice.
	 */
	if (type == KRAKEN_LOCATION_CHANGE_RELOAD || mount) {
		force_reload = TRUE;
	} else if (!kraken_monitor_active ()) {
		force_reload = TRUE;
	} else {
		force_reload = !kraken_directory_is_local (directory);
	}

	if (force_reload) {
        file = kraken_directory_get_corresponding_file (directory);
        kraken_file_invalidate_all_attributes (file);
        kraken_file_unref (file);

        kraken_directory_force_reload (directory);

	}

        kraken_directory_unref (directory);

	if (parent_selection != NULL) {
		g_list_free_full (parent_selection, g_object_unref);
	}

        /* Set current_bookmark scroll pos */
        if (slot->current_location_bookmark != NULL &&
            slot->content_view != NULL) {
                current_pos = kraken_view_get_first_visible_file (slot->content_view);
                kraken_bookmark_set_scroll_pos (slot->current_location_bookmark, current_pos);
                g_free (current_pos);
        }

	/* Get the info needed for view selection */

        slot->determine_view_file = kraken_file_get (location);
	g_assert (slot->determine_view_file != NULL);

	/* if the currently viewed file is marked gone while loading the new location,
	 * this ensures that the window isn't destroyed */
        cancel_viewed_file_changed_callback (slot);

	kraken_file_call_when_ready (slot->determine_view_file,
				       KRAKEN_FILE_ATTRIBUTE_INFO |
				       KRAKEN_FILE_ATTRIBUTE_MOUNT,
                                       got_file_info_for_view_selection_callback,
				       slot);
}

typedef struct {
	GCancellable *cancellable;
	KrakenWindowSlot *slot;
} MountNotMountedData;

static void
mount_not_mounted_callback (GObject *source_object,
			    GAsyncResult *res,
			    gpointer user_data)
{
	MountNotMountedData *data;
	KrakenWindowSlot *slot;
	GError *error;
	GCancellable *cancellable;

	data = user_data;
	slot = data->slot;
	cancellable = data->cancellable;
	g_free (data);

	if (g_cancellable_is_cancelled (cancellable)) {
		/* Cancelled, don't call back */
		g_object_unref (cancellable);
		return;
	}

	slot->mount_cancellable = NULL;

	slot->determine_view_file = kraken_file_get (slot->pending_location);

	error = NULL;
	if (!g_file_mount_enclosing_volume_finish (G_FILE (source_object), res, &error)) {
		slot->mount_error = error;
		got_file_info_for_view_selection_callback (slot->determine_view_file, slot);
		slot->mount_error = NULL;
		g_error_free (error);
	} else {
		kraken_file_invalidate_all_attributes (slot->determine_view_file);
		kraken_file_call_when_ready (slot->determine_view_file,
					       KRAKEN_FILE_ATTRIBUTE_INFO,
					       got_file_info_for_view_selection_callback,
					       slot);
	}

	g_object_unref (cancellable);
}

static void
got_file_info_for_view_selection_callback (KrakenFile *file,
					   gpointer callback_data)
{
        GError *error;
	char *view_id;
	char *mimetype;
	KrakenWindow *window;
	KrakenWindowSlot *slot;
	KrakenFile *parent_file, *tmp;
	GFile *location;
	GMountOperation *mount_op;
	MountNotMountedData *data;
	KrakenApplication *app;

	slot = callback_data;
	window = kraken_window_slot_get_window (slot);

	g_assert (slot->determine_view_file == file);
	slot->determine_view_file = NULL;

	if (slot->mount_error) {
		error = slot->mount_error;
	} else {
		error = kraken_file_get_file_info_error (file);
	}

	if (error && error->domain == G_IO_ERROR && error->code == G_IO_ERROR_NOT_MOUNTED &&
	    !slot->tried_mount) {
		slot->tried_mount = TRUE;

		mount_op = gtk_mount_operation_new (GTK_WINDOW (window));
		g_mount_operation_set_password_save (mount_op, G_PASSWORD_SAVE_FOR_SESSION);
		location = kraken_file_get_location (file);
		data = g_new0 (MountNotMountedData, 1);
		data->cancellable = g_cancellable_new ();
		data->slot = slot;
		slot->mount_cancellable = data->cancellable;
		g_file_mount_enclosing_volume (location, 0, mount_op, slot->mount_cancellable,
					       mount_not_mounted_callback, data);
		g_object_unref (location);
		g_object_unref (mount_op);

		kraken_file_unref (file);

		return;
	}

	parent_file = kraken_file_get_parent (file);
	if ((parent_file != NULL) &&
	    kraken_file_get_file_type (file) == G_FILE_TYPE_REGULAR) {
		if (slot->pending_selection != NULL) {
			g_list_free_full (slot->pending_selection, (GDestroyNotify) kraken_file_unref);
		}

		g_clear_object (&slot->pending_location);
		g_free (slot->pending_scroll_to);

		slot->pending_location = kraken_file_get_parent_location (file);
		slot->pending_selection = g_list_prepend (NULL, kraken_file_ref (file));
		slot->determine_view_file = parent_file;
		slot->pending_scroll_to = kraken_file_get_uri (file);

		kraken_file_invalidate_all_attributes (slot->determine_view_file);
		kraken_file_call_when_ready (slot->determine_view_file,
					       KRAKEN_FILE_ATTRIBUTE_INFO,
					       got_file_info_for_view_selection_callback,
					       slot);

		kraken_file_unref (file);

		return;
	}

	kraken_file_unref (parent_file);
	location = slot->pending_location;

	view_id = NULL;

        if (error == NULL ||
	    (error->domain == G_IO_ERROR && error->code == G_IO_ERROR_NOT_SUPPORTED)) {
		/* We got the information we need, now pick what view to use: */

		mimetype = kraken_file_get_mime_type (file);

		/* Look in metadata for view */
		if (kraken_global_preferences_get_inherit_folder_viewer_preference ()) {
        if (kraken_global_preferences_get_ignore_view_metadata ()) {
        view_id = g_strdup (kraken_window_get_ignore_meta_view_id (window));
        } else {
            parent_file = file;
            kraken_file_ref(parent_file); // Do this once for the initial file
            while (parent_file) {
                view_id = kraken_file_get_metadata (parent_file, KRAKEN_METADATA_KEY_DEFAULT_VIEW, NULL);
                tmp = kraken_file_get_parent (parent_file);
                kraken_file_unref(parent_file);
                if (view_id != NULL) {
                    parent_file = NULL;
                } else {
                    parent_file = tmp;
                }
            }
        }
    } else {
        view_id = kraken_global_preferences_get_ignore_view_metadata () ? g_strdup (kraken_window_get_ignore_meta_view_id (window)) :
                                                                        kraken_file_get_metadata (file, KRAKEN_METADATA_KEY_DEFAULT_VIEW, NULL);
    }

    if (view_id != NULL &&
		    !kraken_view_factory_view_supports_uri (view_id,
							      location,
							      kraken_file_get_file_type (file),
							      mimetype)) {
			g_free (view_id);
			view_id = NULL;
		}

		/* Otherwise, use default */
		if (view_id == NULL) {
            gchar *name, *uri;
            name = kraken_file_get_name (file);
            uri = kraken_file_get_uri (file);

            if (g_strcmp0 (name, "x-kraken-search") == 0) {
                view_id = g_strdup (KRAKEN_LIST_VIEW_IID);
            } else if (eel_uri_is_desktop (uri)) {
                view_id = kraken_global_preferences_get_desktop_iid ();
            } else {
                view_id = kraken_global_preferences_get_default_folder_viewer_preference_as_iid ();
            }

            g_free (uri);
            g_free (name);

			if (view_id != NULL &&
			    !kraken_view_factory_view_supports_uri (view_id,
								      location,
								      kraken_file_get_file_type (file),
								      mimetype)) {
				g_free (view_id);
				view_id = NULL;
			}
		}

		g_free (mimetype);
	}

	if (view_id != NULL) {
		create_content_view (slot, view_id);
		g_free (view_id);

		report_callback (slot, NULL);
	} else {
		if (!report_callback (slot, error)) {
			display_view_selection_failure (window, file,
							location, error);
		}

		if (!gtk_widget_get_visible (GTK_WIDGET (window))) {
			/* Destroy never-had-a-chance-to-be-seen window. This case
			 * happens when a new window cannot display its initial URI.
			 */
			/* if this is the only window, we don't want to quit, so we redirect it to home */

			app = kraken_application_get_singleton ();

			if (g_list_length (gtk_application_get_windows (GTK_APPLICATION (app))) == 1) {
				/* the user could have typed in a home directory that doesn't exist,
				   in which case going home would cause an infinite loop, so we
				   better test for that */

				if (!kraken_is_root_directory (location)) {
					if (!kraken_is_home_directory (location)) {
						kraken_window_slot_go_home (slot, FALSE);
					} else {
						GFile *root;

						root = g_file_new_for_path ("/");
						/* the last fallback is to go to a known place that can't be deleted! */
						kraken_window_slot_open_location (slot, location, 0);
						g_object_unref (root);
					}
				} else {
					gtk_widget_destroy (GTK_WIDGET (window));
				}
			} else {
				/* Since this is a window, destroying it will also unref it. */
				gtk_widget_destroy (GTK_WIDGET (window));
			}
		} else {
			/* Clean up state of already-showing window */
			end_location_change (slot);

			/* TODO? shouldn't we call
			 *   cancel_viewed_file_changed_callback (slot);
			 * at this point, or in end_location_change()
			 */
			/* We're missing a previous location (if opened location
			 * in a new tab) so close it and return */
			if (slot->location == NULL) {
				kraken_window_pane_close_slot (slot->pane, slot);
			} else {
				/* We disconnected this, so we need to re-connect it */
				KrakenFile *viewed_file;
				viewed_file = kraken_file_get (slot->location);
				kraken_window_slot_set_viewed_file (slot, viewed_file);
				kraken_file_monitor_add (viewed_file, &slot->viewed_file, 0);
				g_signal_connect_object (viewed_file, "changed",
							 G_CALLBACK (viewed_file_changed_callback), slot, 0);
				kraken_file_unref (viewed_file);

                gchar *path = kraken_file_get_path (file);
                KRAKEN_WINDOW_GET_CLASS (window)->prompt_for_location(window, path);
                g_free (path);

				/* Leave the location bar showing the bad location that the user
				 * typed (or maybe achieved by dragging or something). Many times
				 * the mistake will just be an easily-correctable typo. The user
				 * can choose "Refresh" to get the original URI back in the location bar.
				 */
			}
		}
	}

	kraken_file_unref (file);
}

/* Load a view into the window, either reusing the old one or creating
 * a new one. This happens when you want to load a new location, or just
 * switch to a different view.
 * If pending_location is set we're loading a new location and
 * pending_location/selection will be used. If not, we're just switching
 * view, and the current location will be used.
 */
static void
create_content_view (KrakenWindowSlot *slot,
                     const char     *view_id)
{
    KrakenWindow *window;
    KrakenView *view;
    GList *selection;

    window = kraken_window_slot_get_window (slot);

    if (slot->content_view != NULL &&
        g_strcmp0 (kraken_view_get_view_id (slot->content_view),
        view_id) == 0) {
        /* reuse existing content view */
        view = slot->content_view;
        slot->new_content_view = view;
        g_object_ref (view);
    } else {
        /* create a new content view */
        view = kraken_view_factory_create (view_id, slot);
        slot->new_content_view = view;
        kraken_window_connect_content_view (window, slot->new_content_view);
    }

    if (KRAKEN_IS_DESKTOP_WINDOW (window)) {
        KrakenDesktopDirectory *directory;

        directory = KRAKEN_DESKTOP_DIRECTORY (kraken_directory_get (slot->pending_location));
        directory->display_number = kraken_desktop_window_get_monitor (KRAKEN_DESKTOP_WINDOW (window));

        kraken_directory_unref (KRAKEN_DIRECTORY (directory));
    }

    /* Actually load the pending location and selection: */

    if (slot->pending_location != NULL) {
        load_new_location (slot,
                           slot->pending_location,
                           slot->pending_selection,
                           FALSE,
                           TRUE);

        g_list_free_full (slot->pending_selection, g_object_unref);
        slot->pending_selection = NULL;
    } else if (slot->location != NULL) {
        selection = kraken_view_get_selection (slot->content_view);
        load_new_location (slot,
                           slot->location,
                           selection,
                           FALSE,
                           TRUE);
        g_list_free_full (selection, g_object_unref);
    } else {
        /* Something is busted, there was no location to load.
           Just load the homedir. */
        kraken_window_slot_go_home (slot, FALSE);

    }
}

static void
load_new_location (KrakenWindowSlot *slot,
		   GFile *location,
		   GList *selection,
		   gboolean tell_current_content_view,
		   gboolean tell_new_content_view)
{
	GList *selection_copy;
	KrakenView *view;

	g_assert (slot != NULL);
	g_assert (location != NULL);

	selection_copy = eel_g_object_list_copy (selection);
	view = NULL;

	/* Note, these may recurse into report_load_underway */
        if (slot->content_view != NULL && tell_current_content_view) {
		view = slot->content_view;
		kraken_view_load_location (slot->content_view, location);
        }

        if (slot->new_content_view != NULL && tell_new_content_view &&
	    (!tell_current_content_view ||
	     slot->new_content_view != slot->content_view) ) {
		view = slot->new_content_view;
		kraken_view_load_location (slot->new_content_view, location);
        }
	if (view != NULL) {
		/* slot->new_content_view might have changed here if
		   report_load_underway was called from load_location */
		kraken_view_set_selection (view, selection_copy);
	}

	g_list_free_full (selection_copy, g_object_unref);
}

/* A view started to load the location its viewing, either due to
 * a load_location request, or some internal reason. Expect
 * a matching load_compete later
 */
void
kraken_window_report_load_underway (KrakenWindow *window,
				      KrakenView *view)
{
	KrakenWindowSlot *slot;

	g_assert (KRAKEN_IS_WINDOW (window));

	if (window->details->temporarily_ignore_view_signals) {
		return;
	}

	slot = kraken_window_get_slot_for_view (window, view);
	g_assert (slot != NULL);

	if (view == slot->new_content_view) {
		location_has_really_changed (slot);
	} else {
		kraken_window_slot_set_allow_stop (slot, TRUE);
	}
}

static void
kraken_window_emit_location_change (KrakenWindow *window,
				      GFile *location)
{
	char *uri;

	uri = g_file_get_uri (location);
	g_signal_emit_by_name (window, "loading_uri", uri);
	g_free (uri);
}

static void
kraken_window_slot_emit_location_change (KrakenWindowSlot *slot,
					   GFile *from,
					   GFile *to)
{
	char *from_uri = NULL;
	char *to_uri = NULL;

	if (from != NULL)
		from_uri = g_file_get_uri (from);
	if (to != NULL)
		to_uri = g_file_get_uri (to);
	g_signal_emit_by_name (slot, "location-changed", from_uri, to_uri);
	g_free (to_uri);
	g_free (from_uri);
}

/* reports location change to window's "loading-uri" clients, i.e.
 * sidebar panels [used when switching tabs]. It will emit the pending
 * location, or the existing location if none is pending.
 */
void
kraken_window_report_location_change (KrakenWindow *window)
{
	KrakenWindowSlot *slot;
	GFile *location;

	slot = kraken_window_get_active_slot (window);
	g_assert (KRAKEN_IS_WINDOW_SLOT (slot));

	location = NULL;

	if (slot->pending_location != NULL) {
		location = slot->pending_location;
	}

	if (location == NULL && slot->location != NULL) {
		location = slot->location;
	}

	if (location != NULL) {
		kraken_window_emit_location_change (window, location);
	}
}

static void
real_setup_loading_floating_bar (KrakenWindowSlot *slot)
{
	gboolean disable_chrome;

	g_object_get (kraken_window_slot_get_window (slot),
		      "disable-chrome", &disable_chrome,
		      NULL);

	if (disable_chrome) {
		gtk_widget_hide (slot->floating_bar);
		return;
	}

	kraken_floating_bar_set_label (KRAKEN_FLOATING_BAR (slot->floating_bar),
					 KRAKEN_IS_SEARCH_DIRECTORY (kraken_view_get_model (slot->content_view)) ?
					 _("Searching...") : _("Loading..."));
	kraken_floating_bar_set_show_spinner (KRAKEN_FLOATING_BAR (slot->floating_bar),
						TRUE);
	kraken_floating_bar_add_action (KRAKEN_FLOATING_BAR (slot->floating_bar),
					  GTK_STOCK_STOP,
					  KRAKEN_FLOATING_BAR_ACTION_ID_STOP);

	gtk_widget_set_halign (slot->floating_bar, GTK_ALIGN_END);
	gtk_widget_show (slot->floating_bar);
}

static gboolean
setup_loading_floating_bar_timeout_cb (gpointer user_data)
{
	KrakenWindowSlot *slot = user_data;

	slot->loading_timeout_id = 0;
	real_setup_loading_floating_bar (slot);

	return FALSE;
}

static void
setup_loading_floating_bar (KrakenWindowSlot *slot)
{
	/* setup loading overlay */
	if (slot->set_status_timeout_id != 0) {
		g_source_remove (slot->set_status_timeout_id);
		slot->set_status_timeout_id = 0;
	}

	if (slot->loading_timeout_id != 0) {
		g_source_remove (slot->loading_timeout_id);
		slot->loading_timeout_id = 0;
	}

	slot->loading_timeout_id =
		g_timeout_add (500, setup_loading_floating_bar_timeout_cb, slot);
}

/* This is called when we have decided we can actually change to the new view/location situation. */
static void
location_has_really_changed (KrakenWindowSlot *slot)
{
	KrakenWindow *window;
	GtkWidget *widget;
	GFile *location_copy;

	window = kraken_window_slot_get_window (slot);

	if (slot->new_content_view != NULL) {
		widget = GTK_WIDGET (slot->new_content_view);
		/* Switch to the new content view. */
		if (gtk_widget_get_parent (widget) == NULL) {
			kraken_window_slot_set_content_view_widget (slot, slot->new_content_view);
		}
		g_object_unref (slot->new_content_view);
		slot->new_content_view = NULL;
	}

      if (slot->pending_location != NULL) {
		/* Tell the window we are finished. */
		update_for_new_location (slot);
	}

	location_copy = NULL;
	if (slot->location != NULL) {
		location_copy = g_object_ref (slot->location);
	}

	free_location_change (slot);

	if (location_copy != NULL) {
		if (slot == kraken_window_get_active_slot (window)) {
			kraken_window_emit_location_change (window, location_copy);
		}

		g_object_unref (location_copy);
	}

	setup_loading_floating_bar (slot);
}

static void
slot_add_extension_extra_widgets (KrakenWindowSlot *slot)
{
	GList *providers, *l;
	GtkWidget *widget;
	char *uri;
	KrakenWindow *window;

	providers = kraken_module_get_extensions_for_type (KRAKEN_TYPE_LOCATION_WIDGET_PROVIDER);
	window = kraken_window_slot_get_window (slot);

	uri = g_file_get_uri (slot->location);
	for (l = providers; l != NULL; l = l->next) {
		KrakenLocationWidgetProvider *provider;

		provider = KRAKEN_LOCATION_WIDGET_PROVIDER (l->data);
		widget = kraken_location_widget_provider_get_widget (provider, uri, GTK_WIDGET (window));
		if (widget != NULL) {
			kraken_window_slot_add_extra_location_widget (slot, widget);
		}
	}
	g_free (uri);

	kraken_module_extension_list_free (providers);
}

static void
kraken_window_slot_show_x_content_bar (KrakenWindowSlot *slot, GMount *mount, const char **x_content_types)
{
	unsigned int n;

	g_assert (KRAKEN_IS_WINDOW_SLOT (slot));

	for (n = 0; x_content_types[n] != NULL; n++) {
		GAppInfo *default_app;

		/* skip blank media; the burn:/// location will provide it's own cluebar */
		if (g_str_has_prefix (x_content_types[n], "x-content/blank-")) {
			continue;
		}

		/* don't show the cluebar for windows software */
		if (g_content_type_is_a (x_content_types[n], "x-content/win32-software")) {
			continue;
		}

		/* only show the cluebar if a default app is available */
		default_app = g_app_info_get_default_for_type (x_content_types[n], FALSE);
		if (default_app != NULL)  {
			GtkWidget *bar;
			bar = kraken_x_content_bar_new (mount, x_content_types[n]);
			gtk_widget_show (bar);
			kraken_window_slot_add_extra_location_widget (slot, bar);
			g_object_unref (default_app);
		}
	}
}

static void
kraken_window_slot_show_trash_bar (KrakenWindowSlot *slot)
{
	GtkWidget *bar;
	KrakenView *view;

	view = kraken_window_slot_get_current_view (slot);
	bar = kraken_trash_bar_new (view);
	gtk_widget_show (bar);

	kraken_window_slot_add_extra_location_widget (slot, bar);
}

static void
maybe_show_interesting_folder_bar (KrakenWindowSlot *slot)
{
    GtkWidget *bar = kraken_interesting_folder_bar_new_for_location (kraken_window_slot_get_current_view(slot),
                                                                   slot->location);

    if (bar) {
        gtk_widget_show (bar);
        kraken_window_slot_add_extra_location_widget (slot, bar);
    }
}

typedef struct {
	KrakenWindowSlot *slot;
	GCancellable *cancellable;
	GMount *mount;
} FindMountData;

static void
found_content_type_cb (const char **x_content_types,
		       gpointer user_data)
{
	KrakenWindowSlot *slot;
	FindMountData *data = user_data;

	if (g_cancellable_is_cancelled (data->cancellable)) {
		goto out;
	}

	slot = data->slot;

	if (x_content_types != NULL && x_content_types[0] != NULL) {
		kraken_window_slot_show_x_content_bar (slot, data->mount, x_content_types);
	}

	slot->find_mount_cancellable = NULL;

 out:
	g_object_unref (data->mount);
	g_object_unref (data->cancellable);
	g_free (data);
}

static void
found_mount_cb (GObject *source_object,
		GAsyncResult *res,
		gpointer user_data)
{
	FindMountData *data = user_data;
	GMount *mount;

	if (g_cancellable_is_cancelled (data->cancellable)) {
		goto out;
	}

	mount = g_file_find_enclosing_mount_finish (G_FILE (source_object),
						    res,
						    NULL);
	if (mount != NULL) {
		data->mount = mount;
		
		if (g_settings_get_boolean (kraken_preferences, KRAKEN_PREFERENCES_MEDIA_HANDLING_DETECT_CONTENT)) {
			kraken_get_x_content_types_for_mount_async (mount,
											found_content_type_cb,
											data->cancellable,
											data);
		}

		return;
	}

	data->slot->find_mount_cancellable = NULL;

 out:
	g_object_unref (data->cancellable);
	g_free (data);
}

/* Handle the changes for the KrakenWindow itself. */
static void
update_for_new_location (KrakenWindowSlot *slot)
{
	KrakenWindow *window;
        GFile *new_location;
        KrakenFile *file;
	KrakenDirectory *directory;
	gboolean location_really_changed;
	FindMountData *data;

	window = kraken_window_slot_get_window (slot);
	new_location = slot->pending_location;
	slot->pending_location = NULL;

	set_displayed_location (slot, new_location);

	update_history (slot, slot->location_change_type, new_location);

	location_really_changed =
		slot->location == NULL ||
		!g_file_equal (slot->location, new_location);

	kraken_window_slot_emit_location_change (slot, slot->location, new_location);

        /* Set the new location. */
	g_clear_object (&slot->location);
	slot->location = new_location;

        /* Create a KrakenFile for this location, so we can catch it
         * if it goes away.
         */
	cancel_viewed_file_changed_callback (slot);
	file = kraken_file_get (slot->location);
	kraken_window_slot_set_viewed_file (slot, file);
	slot->viewed_file_seen = !kraken_file_is_not_yet_confirmed (file);
	slot->viewed_file_in_trash = kraken_file_is_in_trash (file);
	kraken_file_monitor_add (file, &slot->viewed_file, 0);
	g_signal_connect_object (file, "changed",
				 G_CALLBACK (viewed_file_changed_callback), slot, 0);
        kraken_file_unref (file);

	if (slot == kraken_window_get_active_slot (window)) {
		/* Sync up and zoom action states */
		kraken_window_pane_sync_up_actions (slot->pane);
		kraken_window_sync_zoom_widgets (window);
        kraken_window_sync_bookmark_action (window);
        kraken_window_sync_view_type (window);
        kraken_window_sync_thumbnail_action(window);
        kraken_window_sync_create_folder_button (window);

		/* Load menus from kraken extensions for this location */
		kraken_window_load_extension_menus (window);
	}

	if (location_really_changed) {
        kraken_window_clear_secondary_pane_location (window);
		kraken_window_slot_remove_extra_location_widgets (slot);

		directory = kraken_directory_get (slot->location);

		if (kraken_directory_is_in_trash (directory)) {
			kraken_window_slot_show_trash_bar (slot);
		}

        maybe_show_interesting_folder_bar (slot);

        kraken_window_slot_check_bad_cache_bar (slot);

		/* need the mount to determine if we should put up the x-content cluebar */
		if (slot->find_mount_cancellable != NULL) {
			g_cancellable_cancel (slot->find_mount_cancellable);
			slot->find_mount_cancellable = NULL;
		}

		data = g_new (FindMountData, 1);
		data->slot = slot;
		data->cancellable = g_cancellable_new ();
		data->mount = NULL;

		slot->find_mount_cancellable = data->cancellable;
		g_file_find_enclosing_mount_async (slot->location,
						   G_PRIORITY_DEFAULT,
						   data->cancellable,
						   found_mount_cb,
						   data);

		kraken_directory_unref (directory);

		slot_add_extension_extra_widgets (slot);
	}

	kraken_window_slot_update_title (slot);
	kraken_window_slot_update_icon (slot);

	if (slot == slot->pane->active_slot) {
		kraken_window_pane_sync_location_widgets (slot->pane);

		if (location_really_changed) {
			kraken_window_pane_sync_search_widgets (slot->pane);
		}
	}

    kraken_window_sync_menu_bar (window);
}

/* A location load previously announced by load_underway
 * has been finished */
void
kraken_window_report_load_complete (KrakenWindow *window,
				      KrakenView *view)
{
	KrakenWindowSlot *slot;

	g_assert (KRAKEN_IS_WINDOW (window));

	if (window->details->temporarily_ignore_view_signals) {
		return;
	}

	slot = kraken_window_get_slot_for_view (window, view);
	g_assert (slot != NULL);

	/* Only handle this if we're expecting it.
	 * Don't handle it if its from an old view we've switched from */
	if (view == slot->content_view) {
		if (slot->pending_scroll_to != NULL) {
			kraken_view_scroll_to_file (slot->content_view,
						      slot->pending_scroll_to);
		}
		end_location_change (slot);
	}
}

static void
remove_loading_floating_bar (KrakenWindowSlot *slot)
{
	if (slot->loading_timeout_id != 0) {
		g_source_remove (slot->loading_timeout_id);
		slot->loading_timeout_id = 0;
	}

	gtk_widget_hide (slot->floating_bar);
	kraken_floating_bar_cleanup_actions (KRAKEN_FLOATING_BAR (slot->floating_bar));
}

static void
end_location_change (KrakenWindowSlot *slot)
{
	char *uri;

	uri = kraken_window_slot_get_location_uri (slot);
	if (uri) {
		DEBUG ("Finished loading window for uri %s", uri);
		g_free (uri);
	}

	kraken_window_slot_set_allow_stop (slot, FALSE);
	remove_loading_floating_bar (slot);

        /* Now we can free pending_scroll_to, since the load_complete
         * callback already has been emitted.
         */
	g_free (slot->pending_scroll_to);
	slot->pending_scroll_to = NULL;

	free_location_change (slot);
}

static void
free_location_change (KrakenWindowSlot *slot)
{
	KrakenWindow *window;

	window = kraken_window_slot_get_window (slot);

	g_clear_object (&slot->pending_location);
	g_list_free_full (slot->pending_selection, g_object_unref);
	slot->pending_selection = NULL;

        /* Don't free pending_scroll_to, since thats needed until
         * the load_complete callback.
         */

	if (slot->mount_cancellable != NULL) {
		g_cancellable_cancel (slot->mount_cancellable);
		slot->mount_cancellable = NULL;
	}

        if (slot->determine_view_file != NULL) {
		kraken_file_cancel_call_when_ready
			(slot->determine_view_file,
			 got_file_info_for_view_selection_callback, slot);
                slot->determine_view_file = NULL;
        }

        if (slot->new_content_view != NULL) {
		window->details->temporarily_ignore_view_signals = TRUE;
		kraken_view_stop_loading (slot->new_content_view);
		window->details->temporarily_ignore_view_signals = FALSE;

		kraken_window_disconnect_content_view (window, slot->new_content_view);
        	g_object_unref (slot->new_content_view);
                slot->new_content_view = NULL;
        }
}

static void
cancel_location_change (KrakenWindowSlot *slot)
{
	GList *selection;

        if (slot->pending_location != NULL
            && slot->location != NULL
            && slot->content_view != NULL) {

                /* No need to tell the new view - either it is the
                 * same as the old view, in which case it will already
                 * be told, or it is the very pending change we wish
                 * to cancel.
                 */
		selection = kraken_view_get_selection (slot->content_view);
                load_new_location (slot,
				   slot->location,
				   selection,
				   TRUE,
				   FALSE);
		g_list_free_full (selection, g_object_unref);
        }

        end_location_change (slot);
}

static void
display_view_selection_failure (KrakenWindow *window, KrakenFile *file,
				GFile *location, GError *error)
{
	char *full_uri_for_display;
	char *uri_for_display;
	char *error_message;
	char *detail_message;
	char *scheme_string;

	/* Some sort of failure occurred. How 'bout we tell the user? */
	full_uri_for_display = g_file_get_parse_name (location);
	/* Truncate the URI so it doesn't get insanely wide. Note that even
	 * though the dialog uses wrapped text, if the URI doesn't contain
	 * white space then the text-wrapping code is too stupid to wrap it.
	 */
	uri_for_display = eel_str_middle_truncate
		(full_uri_for_display, MAX_URI_IN_DIALOG_LENGTH);
	g_free (full_uri_for_display);

	error_message = NULL;
	detail_message = NULL;
	if (error == NULL) {
		if (kraken_file_is_directory (file)) {
			error_message = g_strdup_printf
				(_("Could not display \"%s\"."),
				 uri_for_display);
			detail_message = g_strdup
				(_("Kraken has no installed viewer capable of displaying the folder."));
		} else {
			error_message = g_strdup_printf
				(_("Could not display \"%s\"."),
				 uri_for_display);
			detail_message = g_strdup
				(_("The location is not a folder."));
		}
	} else if (error->domain == G_IO_ERROR) {
		switch (error->code) {
		case G_IO_ERROR_NOT_FOUND:
			error_message = g_strdup_printf
				(_("Could not find \"%s\"."),
				 uri_for_display);
			detail_message = g_strdup
				(_("Please check the spelling and try again."));
			break;
		case G_IO_ERROR_NOT_SUPPORTED:
			scheme_string = g_file_get_uri_scheme (location);

			error_message = g_strdup_printf (_("Could not display \"%s\"."),
							 uri_for_display);
			if (scheme_string != NULL) {
				detail_message = g_strdup_printf (_("Kraken cannot handle \"%s\" locations."),
								  scheme_string);
			} else {
				detail_message = g_strdup (_("Kraken cannot handle this kind of location."));
			}
			g_free (scheme_string);
			break;
		case G_IO_ERROR_NOT_MOUNTED:
			error_message = g_strdup_printf (_("Could not display \"%s\"."),
							 uri_for_display);
			detail_message = g_strdup (_("Unable to mount the location."));
			break;

		case G_IO_ERROR_PERMISSION_DENIED:
			error_message = g_strdup_printf (_("Could not display \"%s\"."),
							 uri_for_display);
			detail_message = g_strdup (_("Access was denied."));
			break;

		case G_IO_ERROR_HOST_NOT_FOUND:
			/* This case can be hit for user-typed strings like "foo" due to
			 * the code that guesses web addresses when there's no initial "/".
			 * But this case is also hit for legitimate web addresses when
			 * the proxy is set up wrong.
			 */
			error_message = g_strdup_printf (_("Could not display \"%s\", because the host could not be found."),
							 uri_for_display);
			detail_message = g_strdup (_("Check that the spelling is correct and that your proxy settings are correct."));
			break;
		case G_IO_ERROR_CANCELLED:
		case G_IO_ERROR_FAILED_HANDLED:
			g_free (uri_for_display);
			return;

		default:
			break;
		}
	}

	if (error_message == NULL) {
		error_message = g_strdup_printf (_("Could not display \"%s\"."),
						 uri_for_display);
		detail_message = g_strdup_printf (_("Error: %s\nPlease select another viewer and try again."), error->message);
	}

	eel_show_error_dialog (error_message, detail_message, NULL);

	g_free (uri_for_display);
	g_free (error_message);
	g_free (detail_message);
}


void
kraken_window_slot_stop_loading (KrakenWindowSlot *slot)
{
	KrakenWindow *window;

	window = kraken_window_slot_get_window (slot);

	kraken_view_stop_loading (slot->content_view);

	if (slot->new_content_view != NULL) {
		window->details->temporarily_ignore_view_signals = TRUE;
		kraken_view_stop_loading (slot->new_content_view);
		window->details->temporarily_ignore_view_signals = FALSE;
	}

        cancel_location_change (slot);
}

void
kraken_window_slot_set_content_view (KrakenWindowSlot *slot,
				       const char *id)
{
	KrakenFile *file;
	char *uri;

	g_assert (slot != NULL);
	g_assert (slot->location != NULL);
	g_assert (id != NULL);

	uri = kraken_window_slot_get_location_uri (slot);
	DEBUG ("Change view of window %s to %s", uri, id);
	g_free (uri);

	if (kraken_window_slot_content_view_matches_iid (slot, id)) {
        	return;
        }

        end_location_change (slot);

	file = kraken_file_get (slot->location);

    if (kraken_global_preferences_get_ignore_view_metadata ()) {
        kraken_window_set_ignore_meta_view_id (kraken_window_slot_get_window (slot), id);
    } else {
        kraken_file_set_metadata (file, KRAKEN_METADATA_KEY_DEFAULT_VIEW, NULL, id);
    }

    kraken_file_unref (file);

    kraken_window_slot_set_allow_stop (slot, TRUE);

    if (kraken_view_get_selection_count (slot->content_view) == 0) {
            /* If there is no selection, queue a scroll to the same icon that
             * is currently visible */
            slot->pending_scroll_to = kraken_view_get_first_visible_file (slot->content_view);
    }
	slot->location_change_type = KRAKEN_LOCATION_CHANGE_RELOAD;

        create_content_view (slot, id);
}

void
kraken_window_manage_views_close_slot (KrakenWindowSlot *slot)
{
	if (slot->content_view != NULL) {
		kraken_window_disconnect_content_view (kraken_window_slot_get_window (slot),
							 slot->content_view);
	}

	free_location_change (slot);
	cancel_viewed_file_changed_callback (slot);
}

void
kraken_window_back_or_forward (KrakenWindow *window,
				 gboolean back,
				 guint distance,
				 KrakenWindowOpenFlags flags)
{
	KrakenWindowSlot *slot;
	GList *list;
	GFile *location;
        guint len;
        KrakenBookmark *bookmark;
	GFile *old_location;

	slot = kraken_window_get_active_slot (window);
	list = back ? slot->back_list : slot->forward_list;

        len = (guint) g_list_length (list);

        /* If we can't move in the direction at all, just return. */
        if (len == 0)
                return;

        /* If the distance to move is off the end of the list, go to the end
           of the list. */
        if (distance >= len)
                distance = len - 1;

        bookmark = g_list_nth_data (list, distance);
	location = kraken_bookmark_get_location (bookmark);

	if (flags != 0) {
		kraken_window_slot_open_location (slot, location, flags);
	} else {
		char *scroll_pos;

		old_location = kraken_window_slot_get_location (slot);
		scroll_pos = kraken_bookmark_get_scroll_pos (bookmark);
		begin_location_change
			(slot,
			 location, old_location, NULL,
			 back ? KRAKEN_LOCATION_CHANGE_BACK : KRAKEN_LOCATION_CHANGE_FORWARD,
			 distance,
			 scroll_pos,
             FALSE,
			 NULL, NULL);

		g_clear_object (&old_location);
		g_free (scroll_pos);
	}

	g_object_unref (location);
}

/* reload the contents of the window */
void
kraken_window_slot_force_reload (KrakenWindowSlot *slot)
{
	GFile *location;
        char *current_pos;
	GList *selection;

	g_assert (KRAKEN_IS_WINDOW_SLOT (slot));

	if (slot->location == NULL) {
		return;
	}

	/* peek_slot_field (window, location) can be free'd during the processing
	 * of begin_location_change, so make a copy
	 */
	location = g_object_ref (slot->location);
	current_pos = NULL;
	selection = NULL;
	if (slot->content_view != NULL) {
		current_pos = kraken_view_get_first_visible_file (slot->content_view);
		selection = kraken_view_get_selection (slot->content_view);
	}
	begin_location_change
		(slot, location, location, selection,
		 KRAKEN_LOCATION_CHANGE_RELOAD, 0, current_pos,
		 FALSE, NULL, NULL);
        g_free (current_pos);
	g_object_unref (location);
	g_list_free_full (selection, g_object_unref);
}

static void
clear_thumbnails_for_view (KrakenView *view)
{
    KrakenDirectory *directory;
    GList *file_list, *l;

    directory = kraken_view_get_model (view);

    file_list = kraken_directory_get_file_list (directory);

    for (l = file_list; l != NULL; l = l->next) {
        KrakenFile *file = KRAKEN_FILE (l->data);

        kraken_file_delete_thumbnail (file);
    }

    kraken_file_list_free (file_list);

    kraken_icon_info_clear_caches ();
}

void
kraken_window_slot_queue_reload (KrakenWindowSlot *slot,
                               gboolean        clear_thumbs)
{
	g_assert (KRAKEN_IS_WINDOW_SLOT (slot));

	if (slot->location == NULL) {
		return;
	}

	if (slot->pending_location != NULL
	    || slot->content_view == NULL
	    || kraken_view_get_loading (slot->content_view)) {
		/* there is a reload in flight */
		slot->needs_reload = TRUE;
		return;
	}

    if (clear_thumbs && !kraken_file_is_in_favorites (slot->viewed_file)) {
        clear_thumbnails_for_view (slot->content_view);
    }

	kraken_window_slot_force_reload (slot);
}

void
kraken_window_slot_check_bad_cache_bar (KrakenWindowSlot *slot)
{
    int show_image_thumbs;
    if (KRAKEN_IS_DESKTOP_WINDOW (kraken_window_slot_get_window (slot)))
        return;

    show_image_thumbs = g_settings_get_enum (kraken_preferences, KRAKEN_PREFERENCES_SHOW_IMAGE_FILE_THUMBNAILS);
    if (show_image_thumbs != KRAKEN_SPEED_TRADEOFF_NEVER &&
        kraken_application_get_cache_bad (kraken_application_get_singleton ()) &&
        !kraken_application_get_cache_problem_ignored (kraken_application_get_singleton ())) {
        if (slot->cache_bar != NULL) {
            gtk_widget_show (slot->cache_bar);
        } else {
            GtkWidget *bad_bar = kraken_thumbnail_problem_bar_new (kraken_window_slot_get_current_view (slot));
            if (bad_bar) {
                gtk_widget_show (bad_bar);
                kraken_window_slot_add_extra_location_widget (slot, bad_bar);
                slot->cache_bar = bad_bar;

                g_object_add_weak_pointer (G_OBJECT (bad_bar), (gpointer) &slot->cache_bar);
            }
        }
    } else {
        if (slot->cache_bar != NULL) {
            gtk_widget_hide (slot->cache_bar);
        }
    }
}
