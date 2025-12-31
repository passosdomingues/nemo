/* -*- Mode: C; indent-tabs-mode: f; c-basic-offset: 4; tab-width: 4 -*- */

/* fm-icon-container.h - the container widget for file manager icons

   Copyright (C) 2002 Sun Microsystems, Inc.

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

   Author: Michael Meeks <michael@ximian.com>
*/

#ifndef KRAKEN_ICON_VIEW_GRID_CONTAINER_H
#define KRAKEN_ICON_VIEW_GRID_CONTAINER_H

#include "kraken-icon-view.h"

#include <libkraken-private/kraken-icon-private.h>

typedef struct KrakenIconViewGridContainer KrakenIconViewGridContainer;
typedef struct KrakenIconViewGridContainerClass KrakenIconViewGridContainerClass;

#define KRAKEN_TYPE_ICON_VIEW_GRID_CONTAINER kraken_icon_view_grid_container_get_type()
#define KRAKEN_ICON_VIEW_GRID_CONTAINER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_ICON_VIEW_GRID_CONTAINER, KrakenIconViewGridContainer))
#define KRAKEN_ICON_VIEW_GRID_CONTAINER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_ICON_VIEW_GRID_CONTAINER, KrakenIconViewGridContainerClass))
#define KRAKEN_IS_ICON_VIEW_GRID_CONTAINER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_ICON_VIEW_GRID_CONTAINER))
#define KRAKEN_IS_ICON_VIEW_GRID_CONTAINER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_ICON_VIEW_GRID_CONTAINER))
#define KRAKEN_ICON_VIEW_GRID_CONTAINER_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_ICON_VIEW_GRID_CONTAINER, KrakenIconViewGridContainerClass))

typedef struct KrakenIconViewGridContainerDetails KrakenIconViewGridContainerDetails;

struct KrakenIconViewGridContainer {
	KrakenIconContainer parent;

	KrakenIconView *view;
	gboolean      sort_for_desktop;
    gboolean      horizontal;
    gboolean      manual_sort_dirty;
    gint          text_ellipsis_limit;

    GQuark       *attributes;
};

struct KrakenIconViewGridContainerClass {
	KrakenIconContainerClass parent_class;
};

GType                  kraken_icon_view_grid_container_get_type         (void);
KrakenIconContainer *kraken_icon_view_grid_container_construct        (KrakenIconViewGridContainer *icon_container,
                                                                   KrakenIconView              *view,
                                                                   gboolean                   is_desktop);
KrakenIconContainer *kraken_icon_view_grid_container_new              (KrakenIconView              *view,
                                                                   gboolean                   is_desktop);
void                   kraken_icon_view_grid_container_set_sort_desktop (KrakenIconViewGridContainer *container,
                                                                       gboolean                   desktop);

#endif /* KRAKEN_ICON_VIEW_GRID_CONTAINER_H */
