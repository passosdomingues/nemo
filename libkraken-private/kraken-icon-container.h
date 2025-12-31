/* -*- Mode: C; indent-tabs-mode: f; c-basic-offset: 4; tab-width: 4 -*- */

/* gnome-icon-container.h - Icon container widget.

   Copyright (C) 1999, 2000 Free Software Foundation
   Copyright (C) 2000 Eazel, Inc.

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

   Authors: Ettore Perazzoli <ettore@gnu.org>, Darin Adler <darin@bentspoon.com>
*/

#ifndef KRAKEN_ICON_CONTAINER_H
#define KRAKEN_ICON_CONTAINER_H

#include <eel/eel-canvas.h>
#include <libkraken-private/kraken-icon-info.h>
#include <libkraken-private/kraken-icon.h>

#define KRAKEN_TYPE_ICON_CONTAINER kraken_icon_container_get_type()
#define KRAKEN_ICON_CONTAINER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_ICON_CONTAINER, KrakenIconContainer))
#define KRAKEN_ICON_CONTAINER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_ICON_CONTAINER, KrakenIconContainerClass))
#define KRAKEN_IS_ICON_CONTAINER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_ICON_CONTAINER))
#define KRAKEN_IS_ICON_CONTAINER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_ICON_CONTAINER))
#define KRAKEN_ICON_CONTAINER_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_ICON_CONTAINER, KrakenIconContainerClass))

/* Initial unpositioned icon value */
#define ICON_UNPOSITIONED_VALUE -1

typedef struct {
	int x;
	int y;
	double scale;
    int monitor;
} KrakenIconPosition;

typedef enum {
	KRAKEN_ICON_LAYOUT_L_R_T_B,
	KRAKEN_ICON_LAYOUT_R_L_T_B,
	KRAKEN_ICON_LAYOUT_T_B_L_R,
	KRAKEN_ICON_LAYOUT_T_B_R_L
} KrakenIconLayoutMode;

typedef enum {
	KRAKEN_ICON_LABEL_POSITION_UNDER,
	KRAKEN_ICON_LABEL_POSITION_BESIDE
} KrakenIconLabelPosition;

#define	KRAKEN_ICON_CONTAINER_TYPESELECT_FLUSH_DELAY 1000000

typedef struct KrakenIconContainerDetails KrakenIconContainerDetails;

typedef struct {
	EelCanvas canvas;
	KrakenIconContainerDetails *details;
} KrakenIconContainer;

typedef struct {
	EelCanvasClass parent_slot;
    gboolean is_grid_container;

	/* Operations on the container. */
	int          (* button_press) 	          (KrakenIconContainer *container,
						   GdkEventButton *event);
	void         (* context_click_background) (KrakenIconContainer *container,
						   GdkEventButton *event);
	void         (* middle_click) 		  (KrakenIconContainer *container,
						   GdkEventButton *event);

	/* Operations on icons. */
	void         (* activate)	  	  (KrakenIconContainer *container,
						   KrakenIconData *data);
	void         (* activate_alternate)       (KrakenIconContainer *container,
						   KrakenIconData *data);
	void         (* activate_previewer)       (KrakenIconContainer *container,
						   GList *files,
						   GArray *locations);
	void         (* context_click_selection)  (KrakenIconContainer *container,
						   GdkEventButton *event);
	void	     (* move_copy_items)	  (KrakenIconContainer *container,
						   const GList *item_uris,
						   GdkPoint *relative_item_points,
						   const char *target_uri,
						   GdkDragAction action,
						   int x,
						   int y);
	void	     (* handle_netscape_url)	  (KrakenIconContainer *container,
						   const char *url,
						   const char *target_uri,
						   GdkDragAction action,
						   int x,
						   int y);
	void	     (* handle_uri_list)    	  (KrakenIconContainer *container,
						   const char *uri_list,
						   const char *target_uri,
						   GdkDragAction action,
						   int x,
						   int y);
	void	     (* handle_text)		  (KrakenIconContainer *container,
						   const char *text,
						   const char *target_uri,
						   GdkDragAction action,
						   int x,
						   int y);
	void	     (* handle_raw)		  (KrakenIconContainer *container,
						   char *raw_data,
						   int length,
						   const char *target_uri,
						   const char *direct_save_uri,
						   GdkDragAction action,
						   int x,
						   int y);

	/* Queries on the container for subclass/client.
	 * These must be implemented. The default "do nothing" is not good enough.
	 */
	char *	     (* get_container_uri)	  (KrakenIconContainer *container);

	/* Queries on icons for subclass/client.
	 * These must be implemented. The default "do nothing" is not
	 * good enough, these are _not_ signals.
	 */
	KrakenIconInfo *(* get_icon_images)     (KrakenIconContainer *container,
						   KrakenIconData *data,
						   int icon_size,
						   gboolean for_drag_accept,
						   gboolean *has_window_open,
                           gboolean visible);
	void         (* get_icon_text)            (KrakenIconContainer *container,
						   KrakenIconData *data,
						   char **editable_text,
						   char **additional_text,
                           gboolean *pinned,
                           gboolean *fav_unavailable,
						   gboolean include_invisible);
    void         (* update_icon)              (KrakenIconContainer *container,
                                               KrakenIcon          *icon,
                                               gboolean           visible);
	char *       (* get_icon_description)     (KrakenIconContainer *container,
						   KrakenIconData *data);
	int          (* compare_icons)            (KrakenIconContainer *container,
						   KrakenIconData *icon_a,
						   KrakenIconData *icon_b);
	void         (* freeze_updates)           (KrakenIconContainer *container);
	void         (* unfreeze_updates)         (KrakenIconContainer *container);

    gint         (* get_max_layout_lines_for_pango) (KrakenIconContainer *container);
    gint         (* get_max_layout_lines)           (KrakenIconContainer *container);
    gint         (* get_additional_text_line_count) (KrakenIconContainer *container);

	/* Queries on icons for subclass/client.
	 * These must be implemented => These are signals !
	 * The default "do nothing" is not good enough.
	 */
	gboolean     (* can_accept_item)	  (KrakenIconContainer *container,
						   KrakenIconData *target, 
						   const char *item_uri);
	char *       (* get_icon_uri)             (KrakenIconContainer *container,
						   KrakenIconData *data);
	char *       (* get_icon_drop_target_uri) (KrakenIconContainer *container,
						   KrakenIconData *data);

	/* If icon data is NULL, the layout timestamp of the container should be retrieved.
	 * That is the time when the container displayed a fully loaded directory with
	 * all icon positions assigned.
	 *
	 * If icon data is not NULL, the position timestamp of the icon should be retrieved.
	 * That is the time when the file (i.e. icon data payload) was last displayed in a
	 * fully loaded directory with all icon positions assigned.
	 */
	gboolean     (* get_stored_layout_timestamp) (KrakenIconContainer *container,
						      KrakenIconData *data,
						      time_t *time);
	/* If icon data is NULL, the layout timestamp of the container should be stored.
	 * If icon data is not NULL, the position timestamp of the container should be stored.
	 */
	gboolean     (* store_layout_timestamp) (KrakenIconContainer *container,
						 KrakenIconData *data,
						 const time_t *time);

    void         (*lay_down_icons) (KrakenIconContainer *container, GList *icons, double start_y);
    void         (*icon_set_position) (KrakenIconContainer *container, KrakenIcon *icon, double x, double y);
    void         (*move_icon) (KrakenIconContainer *container, KrakenIcon *icon, int x, int y,
                               double scale, gboolean raise, gboolean snap, gboolean update_position);
    void         (*align_icons) (KrakenIconContainer *container);
    void         (*finish_adding_new_icons) (KrakenIconContainer *container);
    void         (*reload_icon_positions) (KrakenIconContainer *container);
    void         (*icon_get_bounding_box) (KrakenIcon *icon,
                                           int *x1_return, int *y1_return,
                                           int *x2_return, int *y2_return,
                                           KrakenIconCanvasItemBoundsUsage usage);
    void         (*set_zoom_level)        (KrakenIconContainer *container, gint new_level);
	/* Notifications for the whole container. */
	void	     (* band_select_started)	  (KrakenIconContainer *container);
	void	     (* band_select_ended)	  (KrakenIconContainer *container);
	void         (* selection_changed) 	  (KrakenIconContainer *container);
	void         (* layout_changed)           (KrakenIconContainer *container);

	/* Notifications for icons. */
	void         (* icon_position_changed)    (KrakenIconContainer *container,
						   KrakenIconData *data,
						   const KrakenIconPosition *position);
	void         (* icon_rename_started)      (KrakenIconContainer *container,
						   GtkWidget *renaming_widget);
	void         (* icon_rename_ended)        (KrakenIconContainer *container,
						   KrakenIconData *data,
						   const char *text);
	void	     (* icon_stretch_started)     (KrakenIconContainer *container,
						   KrakenIconData *data);
	void	     (* icon_stretch_ended)       (KrakenIconContainer *container,
						   KrakenIconData *data);
	int	     (* preview)		  (KrakenIconContainer *container,
						   KrakenIconData *data,
						   gboolean start_flag);
        void         (* icon_added)               (KrakenIconContainer *container,
                                                   KrakenIconData *data);
        void         (* icon_removed)             (KrakenIconContainer *container,
                                                   KrakenIconData *data);
        void         (* cleared)                  (KrakenIconContainer *container);
	gboolean     (* start_interactive_search) (KrakenIconContainer *container);
} KrakenIconContainerClass;

/* GtkObject */
GType             kraken_icon_container_get_type                      (void);
GtkWidget *       kraken_icon_container_new                           (void);


/* adding, removing, and managing icons */
void              kraken_icon_container_clear                         (KrakenIconContainer  *view);
gboolean          kraken_icon_container_icon_is_new_for_monitor       (KrakenIconContainer *container,
                                                                     KrakenIcon          *icon,
                                                                     gint               current_monitor);
gboolean          kraken_icon_container_add                           (KrakenIconContainer  *view,
									 KrakenIconData       *data);
void              kraken_icon_container_layout_now                    (KrakenIconContainer *container);
gboolean          kraken_icon_container_remove                        (KrakenIconContainer  *view,
									 KrakenIconData       *data);
void              kraken_icon_container_for_each                      (KrakenIconContainer  *view,
									 KrakenIconCallback    callback,
									 gpointer                callback_data);
void              kraken_icon_container_request_update                (KrakenIconContainer  *view,
									 KrakenIconData       *data);
void              kraken_icon_container_invalidate_labels             (KrakenIconContainer  *container);
void              kraken_icon_container_request_update_all            (KrakenIconContainer  *container);
void              kraken_icon_container_reveal                        (KrakenIconContainer  *container,
									 KrakenIconData       *data);
gboolean          kraken_icon_container_is_empty                      (KrakenIconContainer  *container);
KrakenIconData *kraken_icon_container_get_first_visible_icon        (KrakenIconContainer  *container);
void              kraken_icon_container_scroll_to_icon                (KrakenIconContainer  *container,
									 KrakenIconData       *data);

void              kraken_icon_container_begin_loading                 (KrakenIconContainer  *container);
void              kraken_icon_container_end_loading                   (KrakenIconContainer  *container,
									 gboolean                all_icons_added);

/* control the layout */
gboolean          kraken_icon_container_is_auto_layout                (KrakenIconContainer  *container);
void              kraken_icon_container_set_auto_layout               (KrakenIconContainer  *container,
									 gboolean                auto_layout);

gboolean          kraken_icon_container_is_keep_aligned               (KrakenIconContainer  *container);
void              kraken_icon_container_set_keep_aligned              (KrakenIconContainer  *container,
									 gboolean                keep_aligned);
void              kraken_icon_container_set_layout_mode               (KrakenIconContainer  *container,
									 KrakenIconLayoutMode  mode);
void              kraken_icon_container_set_horizontal_layout (KrakenIconContainer *container,
                                                             gboolean           horizontal);
gboolean          kraken_icon_container_get_horizontal_layout (KrakenIconContainer *container);
void              kraken_icon_container_set_grid_adjusts (KrakenIconContainer *container,
                                                        gint               h_adjust,
                                                        gint               v_adjust);

void              kraken_icon_container_set_label_position            (KrakenIconContainer  *container,
									 KrakenIconLabelPosition pos);
void              kraken_icon_container_sort                          (KrakenIconContainer  *container);
void              kraken_icon_container_freeze_icon_positions         (KrakenIconContainer  *container);

gint               kraken_icon_container_get_max_layout_lines           (KrakenIconContainer  *container);
gint               kraken_icon_container_get_max_layout_lines_for_pango (KrakenIconContainer  *container);

void              kraken_icon_container_set_highlighted_for_clipboard (KrakenIconContainer  *container,
									 GList                  *clipboard_icon_data);

/* operations on all icons */
void              kraken_icon_container_unselect_all                  (KrakenIconContainer  *view);
void              kraken_icon_container_select_all                    (KrakenIconContainer  *view);


/* operations on the selection */
void              kraken_icon_container_update_selection              (KrakenIconContainer *container);
GList     *       kraken_icon_container_get_selection                 (KrakenIconContainer  *view);
GList     *       kraken_icon_container_peek_selection                (KrakenIconContainer  *view);
gint              kraken_icon_container_get_selection_count           (KrakenIconContainer  *container);
void			  kraken_icon_container_invert_selection				(KrakenIconContainer  *view);
void              kraken_icon_container_set_selection                 (KrakenIconContainer  *view,
									 GList                  *selection);
GArray    *       kraken_icon_container_get_selected_icon_locations   (KrakenIconContainer  *view);
gboolean          kraken_icon_container_has_stretch_handles           (KrakenIconContainer  *container);
gboolean          kraken_icon_container_is_stretched                  (KrakenIconContainer  *container);
void              kraken_icon_container_show_stretch_handles          (KrakenIconContainer  *container);
void              kraken_icon_container_unstretch                     (KrakenIconContainer  *container);
void              kraken_icon_container_start_renaming_selected_item  (KrakenIconContainer  *container,
									 gboolean                select_all);
/* options */
KrakenZoomLevel kraken_icon_container_get_zoom_level                (KrakenIconContainer  *view);
void              kraken_icon_container_set_zoom_level                (KrakenIconContainer  *view,
									 int                     new_zoom_level);
void              kraken_icon_container_set_single_click_mode         (KrakenIconContainer  *container,
									 gboolean                single_click_mode);
void              kraken_icon_container_set_click_to_rename_enabled (KrakenIconContainer *container,
                                                                             gboolean enabled);
void              kraken_icon_container_enable_linger_selection       (KrakenIconContainer  *view,
									 gboolean                enable);
gboolean          kraken_icon_container_get_is_fixed_size             (KrakenIconContainer  *container);
void              kraken_icon_container_set_is_fixed_size             (KrakenIconContainer  *container,
									 gboolean                is_fixed_size);
gboolean          kraken_icon_container_get_is_desktop                (KrakenIconContainer  *container);
void              kraken_icon_container_set_is_desktop                (KrakenIconContainer  *container,
									 gboolean                is_desktop);
gboolean          kraken_icon_container_get_show_desktop_tooltips     (KrakenIconContainer *container);
void              kraken_icon_container_set_show_desktop_tooltips     (KrakenIconContainer *container,
                                                                              gboolean  show_tooltips);
void              kraken_icon_container_reset_scroll_region           (KrakenIconContainer  *container);
void              kraken_icon_container_set_font                      (KrakenIconContainer  *container,
									 const char             *font); 
void              kraken_icon_container_set_font_size_table           (KrakenIconContainer  *container,
									 const int               font_size_table[KRAKEN_ZOOM_LEVEL_LARGEST + 1]);
void              kraken_icon_container_set_margins                   (KrakenIconContainer  *container,
									 int                     left_margin,
									 int                     right_margin,
									 int                     top_margin,
									 int                     bottom_margin);
void              kraken_icon_container_set_use_drop_shadows          (KrakenIconContainer  *container,
									 gboolean                use_drop_shadows);
char*             kraken_icon_container_get_icon_description          (KrakenIconContainer  *container,
                                                                         KrakenIconData       *data);
gboolean          kraken_icon_container_get_allow_moves               (KrakenIconContainer  *container);
void              kraken_icon_container_set_allow_moves               (KrakenIconContainer  *container,
									 gboolean                allow_moves);
void		  kraken_icon_container_set_forced_icon_size		(KrakenIconContainer  *container,
									 int                     forced_icon_size);
void		  kraken_icon_container_set_all_columns_same_width	(KrakenIconContainer  *container,
									 gboolean                all_columns_same_width);

gboolean	  kraken_icon_container_is_layout_rtl			(KrakenIconContainer  *container);
gboolean	  kraken_icon_container_is_layout_vertical		(KrakenIconContainer  *container);

gboolean          kraken_icon_container_get_store_layout_timestamps   (KrakenIconContainer  *container);
void              kraken_icon_container_set_store_layout_timestamps   (KrakenIconContainer  *container,
									 gboolean                store_layout);

void              kraken_icon_container_widget_to_file_operation_position (KrakenIconContainer *container,
									     GdkPoint              *position);

void         kraken_icon_container_setup_tooltip_preference_callback (KrakenIconContainer *container);
void         kraken_icon_container_update_tooltip_text (KrakenIconContainer  *container,
                                                      KrakenIconCanvasItem *item);
gint         kraken_icon_container_get_additional_text_line_count (KrakenIconContainer *container);
void         kraken_icon_container_set_ok_to_load_deferred_attrs (KrakenIconContainer *container,
                                                                gboolean           ok);
#endif /* KRAKEN_ICON_CONTAINER_H */
