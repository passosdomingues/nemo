/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* Kraken - Icon canvas item class for icon container.
 *
 * Copyright (C) 2000 Eazel, Inc.
 *
 * Author: Andy Hertzfeld <andy@eazel.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street - Suite 500,
 * Boston, MA 02110-1335, USA.
 */

#ifndef KRAKEN_ICON_CANVAS_ITEM_H
#define KRAKEN_ICON_CANVAS_ITEM_H

#include <eel/eel-canvas.h>
#include <eel/eel-art-extensions.h>

G_BEGIN_DECLS

#define KRAKEN_TYPE_ICON_CANVAS_ITEM kraken_icon_canvas_item_get_type()
#define KRAKEN_ICON_CANVAS_ITEM(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_ICON_CANVAS_ITEM, KrakenIconCanvasItem))
#define KRAKEN_ICON_CANVAS_ITEM_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_ICON_CANVAS_ITEM, KrakenIconCanvasItemClass))
#define KRAKEN_IS_ICON_CANVAS_ITEM(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_ICON_CANVAS_ITEM))
#define KRAKEN_IS_ICON_CANVAS_ITEM_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_ICON_CANVAS_ITEM))
#define KRAKEN_ICON_CANVAS_ITEM_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_ICON_CANVAS_ITEM, KrakenIconCanvasItemClass))

typedef struct KrakenIconCanvasItem KrakenIconCanvasItem;
typedef struct KrakenIconCanvasItemClass KrakenIconCanvasItemClass;
typedef struct KrakenIconCanvasItemDetails KrakenIconCanvasItemDetails;

struct KrakenIconCanvasItem {
	EelCanvasItem item;
	KrakenIconCanvasItemDetails *details;
	gpointer user_data;
};

struct KrakenIconCanvasItemClass {
	EelCanvasItemClass parent_class;
};

/* not namespaced due to their length */
typedef enum {
	BOUNDS_USAGE_FOR_LAYOUT,
	BOUNDS_USAGE_FOR_ENTIRE_ITEM,
	BOUNDS_USAGE_FOR_DISPLAY
} KrakenIconCanvasItemBoundsUsage;

/* GObject */
GType       kraken_icon_canvas_item_get_type                 (void);

/* attributes */
void        kraken_icon_canvas_item_set_image                (KrakenIconCanvasItem       *item,
								GdkPixbuf                    *image);
cairo_surface_t* kraken_icon_canvas_item_get_drag_surface    (KrakenIconCanvasItem       *item);
void        kraken_icon_canvas_item_set_emblems              (KrakenIconCanvasItem       *item,
								GList                        *emblem_pixbufs);
void        kraken_icon_canvas_item_set_show_stretch_handles (KrakenIconCanvasItem       *item,
								gboolean                      show_stretch_handles);
double      kraken_icon_canvas_item_get_max_text_width       (KrakenIconCanvasItem       *item);
const char *kraken_icon_canvas_item_get_editable_text        (KrakenIconCanvasItem       *icon_item);
void        kraken_icon_canvas_item_set_renaming             (KrakenIconCanvasItem       *icon_item,
								gboolean                      state);

/* geometry and hit testing */
gboolean    kraken_icon_canvas_item_hit_test_rectangle       (KrakenIconCanvasItem       *item,
								EelIRect                      canvas_rect);
gboolean    kraken_icon_canvas_item_hit_test_stretch_handles (KrakenIconCanvasItem       *item,
								gdouble                       world_x,
								gdouble                       world_y,
								GtkCornerType                *corner);
void        kraken_icon_canvas_item_invalidate_label         (KrakenIconCanvasItem       *item);
void        kraken_icon_canvas_item_invalidate_label_size    (KrakenIconCanvasItem       *item);
EelDRect    kraken_icon_canvas_item_get_icon_rectangle       (const KrakenIconCanvasItem *item);
EelDRect    kraken_icon_canvas_item_get_text_rectangle       (KrakenIconCanvasItem       *item,
								gboolean                      for_layout);
void        kraken_icon_canvas_item_get_icon_canvas_rectangle (KrakenIconCanvasItem *item,
                                                             EelIRect *rect);
void        kraken_icon_canvas_item_get_bounds_for_layout    (KrakenIconCanvasItem       *item,
								double *x1, double *y1, double *x2, double *y2);
void        kraken_icon_canvas_item_get_bounds_for_entire_item (KrakenIconCanvasItem       *item,
								  double *x1, double *y1, double *x2, double *y2);
void        kraken_icon_canvas_item_update_bounds            (KrakenIconCanvasItem       *item,
								double i2w_dx, double i2w_dy);
void        kraken_icon_canvas_item_set_is_visible           (KrakenIconCanvasItem       *item,
								gboolean                      visible);
/* whether the entire label text must be visible at all times */
void        kraken_icon_canvas_item_set_entire_text          (KrakenIconCanvasItem       *icon_item,
								gboolean                      entire_text);
gint        kraken_icon_canvas_item_get_fixed_text_height_for_layout (KrakenIconCanvasItem *item);
G_END_DECLS

#endif /* KRAKEN_ICON_CANVAS_ITEM_H */
