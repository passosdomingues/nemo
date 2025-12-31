/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */

/*
 *  Kraken
 *
 *  Copyright (C) 1999, 2000 Red Hat, Inc.
 *  Copyright (C) 1999, 2000, 2001 Eazel, Inc.
 *
 *  Kraken is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  Kraken is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Suite 500, MA 02110-1335, USA.
 *
 *  Authors: Elliot Lee <sopwith@redhat.com>
 *           Darin Adler <darin@bentspoon.com>
 *
 */

#ifndef KRAKEN_WINDOW_PRIVATE_H
#define KRAKEN_WINDOW_PRIVATE_H

#include "kraken-window.h"
#include "kraken-window-slot.h"
#include "kraken-window-pane.h"
#include "kraken-navigation-state.h"
#include "kraken-bookmark-list.h"

#include <libkraken-private/kraken-directory.h>

/* FIXME bugzilla.gnome.org 42575: Migrate more fields into here. */
struct KrakenWindowDetails
{
        GtkWidget *statusbar;
        GtkWidget *menubar;

        GtkWidget *kraken_status_bar;

        GtkUIManager *ui_manager;
        GtkActionGroup *main_action_group; /* owned by ui_manager */
        guint help_message_cid;

        /* Menus. */
        guint extensions_menu_merge_id;
        GtkActionGroup *extensions_menu_action_group;

        GtkActionGroup *bookmarks_action_group;
        GtkActionGroup *toolbar_action_group;
        guint bookmarks_merge_id;
        KrakenBookmarkList *bookmark_list;

	KrakenWindowShowHiddenFilesMode show_hidden_files_mode;

	/* Ensures that we do not react on signals of a
	 * view that is re-used as new view when its loading
	 * is cancelled
	 */
	gboolean temporarily_ignore_view_signals;

        /* available panes, and active pane.
         * Both of them may never be NULL.
         */
        GList *panes;
        KrakenWindowPane *active_pane;

        GtkWidget *content_paned;
        KrakenNavigationState *nav_state;
        
        /* Side Pane */
        int side_pane_width;
        GtkWidget *sidebar;
        gchar *sidebar_id;

        gboolean show_sidebar;

        /* Toolbar */
        GtkWidget *toolbar;

        /* Toolbar holder */
        GtkWidget *toolbar_holder;

        guint extensions_toolbar_merge_id;
        GtkActionGroup *extensions_toolbar_action_group;

        guint menu_hide_delay_id;

        /* split view */
        GtkWidget *split_view_hpane;

        // A closed pane's location, valid until the remaining pane
        // location changes.
        GFile *secondary_pane_last_location;

        gboolean disable_chrome;

        guint sidebar_width_handler_id;

        guint menu_state_changed_id;

        gboolean menu_skip_release;
        gboolean menu_show_queued;

        gchar *ignore_meta_view_id;
        gint ignore_meta_zoom_level;
        GList *ignore_meta_visible_columns;
        GList *ignore_meta_column_order;
        gchar *ignore_meta_sort_column;
        gint ignore_meta_sort_direction;

        gboolean dynamic_menu_entries_current;
};

/* window geometry */
/* Min values are very small, and a Kraken window at this tiny size is *almost*
 * completely unusable. However, if all the extra bits (sidebar, location bar, etc)
 * are turned off, you can see an icon or two at this size. See bug 5946.
 */

#define KRAKEN_WINDOW_MIN_WIDTH		200
#define KRAKEN_WINDOW_MIN_HEIGHT		200
#define KRAKEN_WINDOW_DEFAULT_WIDTH		800
#define KRAKEN_WINDOW_DEFAULT_HEIGHT		550

typedef void (*KrakenBookmarkFailedCallback) (KrakenWindow *window,
                                                KrakenBookmark *bookmark);

void               kraken_window_sync_view_type                    (KrakenWindow    *window);
void               kraken_window_load_extension_menus                  (KrakenWindow    *window);
KrakenWindowPane *kraken_window_get_next_pane                        (KrakenWindow *window);
void               kraken_menus_append_bookmark_to_menu                (KrakenWindow    *window, 
                                                                          KrakenBookmark  *bookmark, 
                                                                          const char        *parent_path,
                                                                          const char        *parent_id,
                                                                          guint              index_in_parent,
                                                                          GtkActionGroup    *action_group,
                                                                          guint              merge_id,
                                                                          GCallback          refresh_callback,
                                                                          KrakenBookmarkFailedCallback failed_callback);

KrakenWindowSlot *kraken_window_get_slot_for_view                    (KrakenWindow *window,
									  KrakenView   *view);

void                 kraken_window_set_active_slot                     (KrakenWindow    *window,
									  KrakenWindowSlot *slot);
void                 kraken_window_set_active_pane                     (KrakenWindow *window,
                                                                          KrakenWindowPane *new_pane);
KrakenWindowPane * kraken_window_get_active_pane                     (KrakenWindow *window);


/* sync window GUI with current slot. Used when changing slots,
 * and when updating the slot state.
 */
void kraken_window_sync_allow_stop       (KrakenWindow *window,
					    KrakenWindowSlot *slot);
void kraken_window_sync_title            (KrakenWindow *window,
					    KrakenWindowSlot *slot);
void kraken_window_sync_zoom_widgets     (KrakenWindow *window);
void kraken_window_sync_menu_bar         (KrakenWindow *window);
void kraken_window_sync_bookmark_action  (KrakenWindow *window);
void kraken_window_sync_thumbnail_action (KrakenWindow *window);

/* window menus */
GtkActionGroup *kraken_window_create_toolbar_action_group (KrakenWindow *window);
void               kraken_window_initialize_actions                    (KrakenWindow    *window);
void               kraken_window_initialize_menus                      (KrakenWindow    *window);
void               kraken_window_finalize_menus                        (KrakenWindow    *window);

void               kraken_window_update_show_hide_ui_elements           (KrakenWindow     *window);

/* window toolbar */
void               kraken_window_close_pane                            (KrakenWindow    *window,
                                                                          KrakenWindowPane *pane);
void               kraken_window_show_location_entry                   (KrakenWindow    *window);

#endif /* KRAKEN_WINDOW_PRIVATE_H */
