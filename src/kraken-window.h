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
/* kraken-window.h: Interface of the main window object */

#ifndef KRAKEN_WINDOW_H
#define KRAKEN_WINDOW_H

#include <gtk/gtk.h>
#include <eel/eel-glib-extensions.h>
#include <libkraken-private/kraken-bookmark.h>
#include <libkraken-private/kraken-search-directory.h>

#include "kraken-navigation-state.h"
#include "kraken-view.h"
#include "kraken-window-types.h"

#define KRAKEN_TYPE_WINDOW kraken_window_get_type()
#define KRAKEN_WINDOW(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_WINDOW, KrakenWindow))
#define KRAKEN_WINDOW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_WINDOW, KrakenWindowClass))
#define KRAKEN_IS_WINDOW(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_WINDOW))
#define KRAKEN_IS_WINDOW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_WINDOW))
#define KRAKEN_WINDOW_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_WINDOW, KrakenWindowClass))

typedef enum {
        KRAKEN_WINDOW_SHOW_HIDDEN_FILES_ENABLE,
        KRAKEN_WINDOW_SHOW_HIDDEN_FILES_DISABLE
} KrakenWindowShowHiddenFilesMode;

typedef enum {
        KRAKEN_WINDOW_NOT_SHOWN,
        KRAKEN_WINDOW_POSITION_SET,
        KRAKEN_WINDOW_SHOULD_SHOW
} KrakenWindowShowState;

typedef enum {
	KRAKEN_WINDOW_OPEN_SLOT_NONE = 0,
	KRAKEN_WINDOW_OPEN_SLOT_APPEND = 1
}  KrakenWindowOpenSlotFlags;

enum {
    SORT_NULL = -1,
    SORT_ASCENDING = 0,
    SORT_DESCENDING = 1
};

#define KRAKEN_WINDOW_SIDEBAR_PLACES "places"
#define KRAKEN_WINDOW_SIDEBAR_TREE "tree"

typedef struct KrakenWindowDetails KrakenWindowDetails;

typedef struct {
        GtkApplicationWindowClass parent_spot;

	/* Function pointers for overriding, without corresponding signals */

        void   (* sync_title) (KrakenWindow *window,
			       KrakenWindowSlot *slot);
        KrakenIconInfo * (* get_icon) (KrakenWindow *window,
                                         KrakenWindowSlot *slot);

        void   (* prompt_for_location) (KrakenWindow *window, const char *initial);
        void   (* close) (KrakenWindow *window);

        /* Signals used only for keybindings */
        void   (* go_up)  (KrakenWindow *window);
	void   (* reload) (KrakenWindow *window);
} KrakenWindowClass;

struct KrakenWindow {
        GtkApplicationWindow parent_object;
        
        KrakenWindowDetails *details;
};

GType            kraken_window_get_type             (void);
KrakenWindow *     kraken_window_new                  (GtkApplication    *application,
                                                   GdkScreen         *screen);
void             kraken_window_close                (KrakenWindow    *window);

void             kraken_window_connect_content_view (KrakenWindow    *window,
						       KrakenView      *view);
void             kraken_window_disconnect_content_view (KrakenWindow    *window,
							  KrakenView      *view);

void             kraken_window_go_to                (KrakenWindow    *window,
                                                       GFile             *location);
void             kraken_window_go_to_tab            (KrakenWindow    *window,
                                                       GFile             *location);
void             kraken_window_go_to_full           (KrakenWindow    *window,
                                                       GFile             *location,
                                                       KrakenWindowGoToCallback callback,
                                                       gpointer           user_data);
void             kraken_window_new_tab              (KrakenWindow    *window);

GtkUIManager *   kraken_window_get_ui_manager       (KrakenWindow    *window);
GtkActionGroup * kraken_window_get_main_action_group (KrakenWindow   *window);
KrakenNavigationState * 
                 kraken_window_get_navigation_state (KrakenWindow    *window);

void                 kraken_window_report_load_complete     (KrakenWindow *window,
                                                               KrakenView *view);

KrakenWindowSlot * kraken_window_get_extra_slot       (KrakenWindow *window);
KrakenWindowShowHiddenFilesMode
                     kraken_window_get_hidden_files_mode (KrakenWindow *window);
void                 kraken_window_set_hidden_files_mode (KrakenWindow *window,
                                                            KrakenWindowShowHiddenFilesMode  mode);
void                 kraken_window_report_load_underway  (KrakenWindow *window,
                                                            KrakenView *view);
void                 kraken_window_view_visible          (KrakenWindow *window,
                                                            KrakenView *view);
GList *              kraken_window_get_panes             (KrakenWindow *window);
KrakenWindowSlot * kraken_window_get_active_slot       (KrakenWindow *window);
void                 kraken_window_push_status           (KrakenWindow *window,
                                                            const char *text);
GtkWidget *          kraken_window_ensure_location_bar   (KrakenWindow *window);
void                 kraken_window_sync_location_widgets (KrakenWindow *window);
void                 kraken_window_sync_search_widgets   (KrakenWindow *window);
void                 kraken_window_grab_focus            (KrakenWindow *window);
void                 kraken_window_sync_create_folder_button (KrakenWindow *window);
void     kraken_window_hide_sidebar         (KrakenWindow *window);
void     kraken_window_show_sidebar         (KrakenWindow *window);
void     kraken_window_back_or_forward      (KrakenWindow *window,
                                               gboolean        back,
                                               guint           distance,
                                               KrakenWindowOpenFlags flags);
void     kraken_window_split_view_on        (KrakenWindow *window);
void     kraken_window_split_view_off       (KrakenWindow *window);
gboolean kraken_window_split_view_showing   (KrakenWindow *window);

gboolean kraken_window_disable_chrome_mapping (GValue *value,
                                                 GVariant *variant,
                                                 gpointer user_data);

void     kraken_window_set_sidebar_id (KrakenWindow *window,
                                    const gchar *id);

const gchar *    kraken_window_get_sidebar_id (KrakenWindow *window);

void    kraken_window_set_show_sidebar (KrakenWindow *window,
                                      gboolean show);

gboolean  kraken_window_get_show_sidebar (KrakenWindow *window);

const gchar *kraken_window_get_ignore_meta_view_id (KrakenWindow *window);
void         kraken_window_set_ignore_meta_view_id (KrakenWindow *window, const gchar *id);
gint         kraken_window_get_ignore_meta_zoom_level (KrakenWindow *window);
void         kraken_window_set_ignore_meta_zoom_level (KrakenWindow *window, gint level);
GList       *kraken_window_get_ignore_meta_visible_columns (KrakenWindow *window);
void         kraken_window_set_ignore_meta_visible_columns (KrakenWindow *window, GList *list);
GList       *kraken_window_get_ignore_meta_column_order (KrakenWindow *window);
void         kraken_window_set_ignore_meta_column_order (KrakenWindow *window, GList *list);
const gchar *kraken_window_get_ignore_meta_sort_column (KrakenWindow *window);
void         kraken_window_set_ignore_meta_sort_column (KrakenWindow *window, const gchar *column);
gint         kraken_window_get_ignore_meta_sort_direction (KrakenWindow *window);
void         kraken_window_set_ignore_meta_sort_direction (KrakenWindow *window, gint direction);

void         kraken_window_clear_secondary_pane_location (KrakenWindow *window);
KrakenWindowOpenFlags kraken_event_get_window_open_flags   (void);

void kraken_window_slot_added (KrakenWindow *window,  KrakenWindowSlot *slot);
void kraken_window_slot_removed (KrakenWindow *window,  KrakenWindowSlot *slot);

#endif
