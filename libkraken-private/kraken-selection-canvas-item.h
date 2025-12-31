/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* Kraken - Canvas item for floating selection.
 *
 * Copyright (C) 1997, 1998, 1999, 2000 Free Software Foundation
 * Copyright (C) 2011 Red Hat Inc.
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
 *
 * Authors: Federico Mena <federico@nuclecu.unam.mx>
 *          Cosimo Cecchi <cosimoc@redhat.com>
 */

#ifndef __KRAKEN_SELECTION_CANVAS_ITEM_H__
#define __KRAKEN_SELECTION_CANVAS_ITEM_H__

#include <eel/eel-canvas.h>

G_BEGIN_DECLS

#define KRAKEN_TYPE_SELECTION_CANVAS_ITEM kraken_selection_canvas_item_get_type()
#define KRAKEN_SELECTION_CANVAS_ITEM(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_SELECTION_CANVAS_ITEM, KrakenSelectionCanvasItem))
#define KRAKEN_SELECTION_CANVAS_ITEM_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_SELECTION_CANVAS_ITEM, KrakenSelectionCanvasItemClass))
#define KRAKEN_IS_SELECTION_CANVAS_ITEM(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_SELECTION_CANVAS_ITEM))
#define KRAKEN_IS_SELECTION_CANVAS_ITEM_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_SELECTION_CANVAS_ITEM))
#define KRAKEN_SELECTION_CANVAS_ITEM_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_SELECTION_CANVAS_ITEM, KrakenSelectionCanvasItemClass))

typedef struct _KrakenSelectionCanvasItem KrakenSelectionCanvasItem;
typedef struct _KrakenSelectionCanvasItemClass KrakenSelectionCanvasItemClass;
typedef struct _KrakenSelectionCanvasItemDetails KrakenSelectionCanvasItemDetails;

struct _KrakenSelectionCanvasItem {
	EelCanvasItem item;
	KrakenSelectionCanvasItemDetails *priv;
	gpointer user_data;
};

struct _KrakenSelectionCanvasItemClass {
	EelCanvasItemClass parent_class;
};

/* GObject */
GType       kraken_selection_canvas_item_get_type                 (void);

void kraken_selection_canvas_item_fade_out (KrakenSelectionCanvasItem *self,
					      guint transition_time);

G_END_DECLS

#endif /* __KRAKEN_SELECTION_CANVAS_ITEM_H__ */
