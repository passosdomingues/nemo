/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* kraken-icon-view.h - interface for icon view of directory.
 *
 * Copyright (C) 2000 Eazel, Inc.
 *
 * The Gnome Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Gnome Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with the Gnome Library; see the file COPYING.LIB.  If not,
 * write to the Free Software Foundation, Inc., 51 Franklin Street - Suite 500,
 * Boston, MA 02110-1335, USA.
 *
 * Authors: John Sullivan <sullivan@eazel.com>
 *
 */

#ifndef KRAKEN_ICON_VIEW_H
#define KRAKEN_ICON_VIEW_H

#include "kraken-view.h"

typedef struct KrakenIconView KrakenIconView;
typedef struct KrakenIconViewClass KrakenIconViewClass;

#define KRAKEN_TYPE_ICON_VIEW kraken_icon_view_get_type()
#define KRAKEN_ICON_VIEW(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_ICON_VIEW, KrakenIconView))
#define KRAKEN_ICON_VIEW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_ICON_VIEW, KrakenIconViewClass))
#define KRAKEN_IS_ICON_VIEW(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_ICON_VIEW))
#define KRAKEN_IS_ICON_VIEW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_ICON_VIEW))
#define KRAKEN_ICON_VIEW_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_ICON_VIEW, KrakenIconViewClass))

#define KRAKEN_ICON_VIEW_ID "OAFIID:Kraken_File_Manager_Icon_View"
#define FM_COMPACT_VIEW_ID "OAFIID:Kraken_File_Manager_Compact_View"

typedef struct KrakenIconViewDetails KrakenIconViewDetails;

struct KrakenIconView {
	KrakenView parent;
	KrakenIconViewDetails *details;
};

struct KrakenIconViewClass {
	KrakenViewClass parent_class;

    gboolean use_grid_container;
};

/* GObject support */
GType   kraken_icon_view_get_type      (void);
int     kraken_icon_view_compare_files (KrakenIconView   *icon_view,
					  KrakenFile *a,
					  KrakenFile *b);
gboolean kraken_icon_view_is_compact   (KrakenIconView *icon_view);

void    kraken_icon_view_register         (void);
void    kraken_icon_view_compact_register (void);

KrakenIconContainer * kraken_icon_view_get_icon_container (KrakenIconView *view);

void    kraken_icon_view_set_sort_criterion_by_sort_type (KrakenIconView     *icon_view,
                                                        KrakenFileSortType  sort_type);
void    kraken_icon_view_set_directory_keep_aligned (KrakenIconView *icon_view,
                                                   KrakenFile *file,
                                                   gboolean keep_aligned);
gchar  *kraken_icon_view_get_directory_sort_by      (KrakenIconView *icon_view, KrakenFile *file);
gboolean kraken_icon_view_get_directory_sort_reversed (KrakenIconView *icon_view, KrakenFile *file);
void    kraken_icon_view_flip_sort_reversed (KrakenIconView *icon_view);
gboolean kraken_icon_view_set_sort_reversed (KrakenIconView *icon_view,
                                          gboolean      new_value,
                                          gboolean      set_metadata);
void   kraken_icon_view_set_directory_horizontal_layout (KrakenIconView *icon_view,
                                                       KrakenFile     *file,
                                                       gboolean      horizontal);
gboolean kraken_icon_view_get_directory_horizontal_layout (KrakenIconView *icon_view,
                                                         KrakenFile     *file);

void kraken_icon_view_set_directory_grid_adjusts (KrakenIconView *icon_view,
                                                KrakenFile     *file,
                                                gint          horizontal,
                                                gint          vertical);
void kraken_icon_view_get_directory_grid_adjusts (KrakenIconView *icon_view,
                                                KrakenFile     *file,
                                                gint         *horizontal,
                                                gint         *vertical);
#endif /* KRAKEN_ICON_VIEW_H */
