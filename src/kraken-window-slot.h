/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   kraken-window-slot.h: Kraken window slot
 
   Copyright (C) 2008 Free Software Foundation, Inc.
  
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
  
   Author: Christian Neumair <cneumair@gnome.org>
*/

#ifndef KRAKEN_WINDOW_SLOT_H
#define KRAKEN_WINDOW_SLOT_H

#include "kraken-view.h"
#include "kraken-window-types.h"
#include "kraken-query-editor.h"

#define KRAKEN_TYPE_WINDOW_SLOT	 (kraken_window_slot_get_type())
#define KRAKEN_WINDOW_SLOT_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), KRAKEN_TYPE_WINDOW_SLOT, KrakenWindowSlotClass))
#define KRAKEN_WINDOW_SLOT(obj)	 (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_WINDOW_SLOT, KrakenWindowSlot))
#define KRAKEN_IS_WINDOW_SLOT(obj)      (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_WINDOW_SLOT))
#define KRAKEN_IS_WINDOW_SLOT_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), KRAKEN_TYPE_WINDOW_SLOT))
#define KRAKEN_WINDOW_SLOT_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), KRAKEN_TYPE_WINDOW_SLOT, KrakenWindowSlotClass))

typedef enum {
	KRAKEN_LOCATION_CHANGE_STANDARD,
	KRAKEN_LOCATION_CHANGE_BACK,
	KRAKEN_LOCATION_CHANGE_FORWARD,
	KRAKEN_LOCATION_CHANGE_RELOAD
} KrakenLocationChangeType;

struct KrakenWindowSlotClass {
	GtkBoxClass parent_class;

	/* wrapped KrakenWindowInfo signals, for overloading */
	void (* active)		(KrakenWindowSlot *slot);
	void (* inactive)	(KrakenWindowSlot *slot);
	void (* changed_pane)	(KrakenWindowSlot *slot);
};

/* Each KrakenWindowSlot corresponds to a location in the window
 * for displaying a KrakenView, i.e. a tab.
 */
struct KrakenWindowSlot {
	GtkBox parent;

	KrakenWindowPane *pane;

	/* slot contains
 	 *  1) an event box containing extra_location_widgets
 	 *  2) the view box for the content view
 	 */
	GtkWidget *extra_location_widgets;

	GtkWidget *view_overlay;
	GtkWidget *floating_bar;
    GtkWidget *cache_bar;
    GtkWidget *no_search_results_box;

	guint set_status_timeout_id;
	guint loading_timeout_id;

	KrakenView *content_view;
	KrakenView *new_content_view;

	/* Information about bookmarks */
	KrakenBookmark *current_location_bookmark;
	KrakenBookmark *last_location_bookmark;

	/* Current location. */
	GFile *location;
	char *title;
	char *status_text;

	KrakenFile *viewed_file;
	gboolean viewed_file_seen;
	gboolean viewed_file_in_trash;

	gboolean allow_stop;

	KrakenQueryEditor *query_editor;
	GtkWidget *query_editor_revealer;
	gulong qe_changed_id;
	gulong qe_cancel_id;

	/* New location. */
	KrakenLocationChangeType location_change_type;
	guint location_change_distance;
	GFile *pending_location;
	char *pending_scroll_to;
	GList *pending_selection;
	KrakenFile *determine_view_file;
	GCancellable *mount_cancellable;
	GError *mount_error;
	gboolean tried_mount;
	KrakenWindowGoToCallback open_callback;
	gpointer open_callback_user_data;

	gboolean needs_reload;

	GCancellable *find_mount_cancellable;

	gboolean visible;

	/* Back/Forward chain, and history list. 
	 * The data in these lists are KrakenBookmark pointers. 
	 */
	GList *back_list, *forward_list;
};

GType   kraken_window_slot_get_type (void);

KrakenWindowSlot * kraken_window_slot_new (KrakenWindowPane *pane);

void    kraken_window_slot_update_title		   (KrakenWindowSlot *slot);
void    kraken_window_slot_update_icon		   (KrakenWindowSlot *slot);
void    kraken_window_slot_set_query_editor_visible	   (KrakenWindowSlot *slot,
							    gboolean            visible);

GFile * kraken_window_slot_get_location		   (KrakenWindowSlot *slot);
char *  kraken_window_slot_get_location_uri		   (KrakenWindowSlot *slot);

void kraken_window_slot_queue_reload (KrakenWindowSlot *slot,
                                    gboolean        clear_thumbs);
void kraken_window_slot_force_reload (KrakenWindowSlot *slot);

/* convenience wrapper without selection and callback/user_data */
#define kraken_window_slot_open_location(slot, location, flags)\
	kraken_window_slot_open_location_full(slot, location, flags, NULL, NULL, NULL)

void kraken_window_slot_open_location_full (KrakenWindowSlot *slot,
					      GFile *location,
					      KrakenWindowOpenFlags flags,
					      GList *new_selection, /* KrakenFile list */
					      KrakenWindowGoToCallback callback,
					      gpointer user_data);

void			kraken_window_slot_stop_loading	      (KrakenWindowSlot	*slot);

void			kraken_window_slot_set_content_view	      (KrakenWindowSlot	*slot,
								       const char		*id);
const char	       *kraken_window_slot_get_content_view_id      (KrakenWindowSlot	*slot);
gboolean		kraken_window_slot_content_view_matches_iid (KrakenWindowSlot	*slot,
								       const char		*iid);

void    kraken_window_slot_go_home			   (KrakenWindowSlot *slot,
							    KrakenWindowOpenFlags flags);
void    kraken_window_slot_go_up                         (KrakenWindowSlot *slot,
							    KrakenWindowOpenFlags flags);
void    kraken_window_slot_set_content_view_widget	   (KrakenWindowSlot *slot,
							    KrakenView       *content_view);
void    kraken_window_slot_set_viewed_file		   (KrakenWindowSlot *slot,
							    KrakenFile      *file);
void    kraken_window_slot_set_allow_stop		   (KrakenWindowSlot *slot,
							    gboolean	    allow_stop);
void    kraken_window_slot_set_status			   (KrakenWindowSlot *slot,
							    const char	 *status,
							    const char   *short_status,
                                gboolean      location_loading);

void    kraken_window_slot_add_extra_location_widget     (KrakenWindowSlot *slot,
							    GtkWidget       *widget);
void    kraken_window_slot_remove_extra_location_widgets (KrakenWindowSlot *slot);

KrakenView * kraken_window_slot_get_current_view     (KrakenWindowSlot *slot);
char           * kraken_window_slot_get_current_uri      (KrakenWindowSlot *slot);
KrakenWindow * kraken_window_slot_get_window           (KrakenWindowSlot *slot);
void           kraken_window_slot_make_hosting_pane_active (KrakenWindowSlot *slot);

gboolean kraken_window_slot_should_close_with_mount (KrakenWindowSlot *slot,
						       GMount *mount);

void kraken_window_slot_clear_forward_list (KrakenWindowSlot *slot);
void kraken_window_slot_clear_back_list    (KrakenWindowSlot *slot);

void kraken_window_slot_check_bad_cache_bar (KrakenWindowSlot *slot);

void kraken_window_slot_set_show_thumbnails (KrakenWindowSlot *slot,
                                           gboolean show_thumbnails);
#endif /* KRAKEN_WINDOW_SLOT_H */
