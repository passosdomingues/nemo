/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   kraken-metadata.h: #defines and other metadata-related info
 
   Copyright (C) 2000 Eazel, Inc.
  
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
  
   Author: John Sullivan <sullivan@eazel.com>
*/

#ifndef KRAKEN_METADATA_H
#define KRAKEN_METADATA_H

/* Keys for getting/setting Kraken metadata. All metadata used in Kraken
 * should define its key here, so we can keep track of the whole set easily.
 * Any updates here needs to be added in kraken-metadata.c too.
 */

#include <glib.h>

/* Per-file */

#define KRAKEN_METADATA_KEY_DEFAULT_VIEW		 	"kraken-default-view"

#define KRAKEN_METADATA_KEY_LOCATION_BACKGROUND_COLOR 	"folder-background-color"
#define KRAKEN_METADATA_KEY_LOCATION_BACKGROUND_IMAGE 	"folder-background-image"

#define KRAKEN_METADATA_KEY_ICON_VIEW_ZOOM_LEVEL       	"kraken-icon-view-zoom-level"
#define KRAKEN_METADATA_KEY_ICON_VIEW_AUTO_LAYOUT      	"kraken-icon-view-auto-layout"
#define KRAKEN_METADATA_KEY_ICON_VIEW_SORT_BY          	"kraken-icon-view-sort-by"
#define KRAKEN_METADATA_KEY_ICON_VIEW_SORT_REVERSED    	"kraken-icon-view-sort-reversed"
#define KRAKEN_METADATA_KEY_ICON_VIEW_KEEP_ALIGNED            "kraken-icon-view-keep-aligned"
#define KRAKEN_METADATA_KEY_ICON_VIEW_LAYOUT_TIMESTAMP	"kraken-icon-view-layout-timestamp"

#define KRAKEN_METADATA_KEY_LIST_VIEW_ZOOM_LEVEL       	"kraken-list-view-zoom-level"
#define KRAKEN_METADATA_KEY_LIST_VIEW_SORT_COLUMN      	"kraken-list-view-sort-column"
#define KRAKEN_METADATA_KEY_LIST_VIEW_SORT_REVERSED    	"kraken-list-view-sort-reversed"
#define KRAKEN_METADATA_KEY_LIST_VIEW_VISIBLE_COLUMNS    	"kraken-list-view-visible-columns"
#define KRAKEN_METADATA_KEY_LIST_VIEW_COLUMN_ORDER    	"kraken-list-view-column-order"

#define KRAKEN_METADATA_KEY_COMPACT_VIEW_ZOOM_LEVEL		"kraken-compact-view-zoom-level"

#define KRAKEN_METADATA_KEY_WINDOW_GEOMETRY			"kraken-window-geometry"
#define KRAKEN_METADATA_KEY_WINDOW_SCROLL_POSITION		"kraken-window-scroll-position"
#define KRAKEN_METADATA_KEY_WINDOW_SHOW_HIDDEN_FILES		"kraken-window-show-hidden-files"
#define KRAKEN_METADATA_KEY_WINDOW_MAXIMIZED			"kraken-window-maximized"
#define KRAKEN_METADATA_KEY_WINDOW_STICKY			"kraken-window-sticky"
#define KRAKEN_METADATA_KEY_WINDOW_KEEP_ABOVE			"kraken-window-keep-above"

#define KRAKEN_METADATA_KEY_SIDEBAR_BACKGROUND_COLOR   	"kraken-sidebar-background-color"
#define KRAKEN_METADATA_KEY_SIDEBAR_BACKGROUND_IMAGE   	"kraken-sidebar-background-image"
#define KRAKEN_METADATA_KEY_SIDEBAR_BUTTONS			"kraken-sidebar-buttons"

#define KRAKEN_METADATA_KEY_ANNOTATION                    "annotation"

#define KRAKEN_METADATA_KEY_ICON_POSITION              	"kraken-icon-position"
#define KRAKEN_METADATA_KEY_ICON_POSITION_TIMESTAMP		"kraken-icon-position-timestamp"
#define KRAKEN_METADATA_KEY_ICON_SCALE                 	"icon-scale"
#define KRAKEN_METADATA_KEY_CUSTOM_ICON                	"custom-icon"
#define KRAKEN_METADATA_KEY_CUSTOM_ICON_NAME                	"custom-icon-name"
#define KRAKEN_METADATA_KEY_EMBLEMS				"emblems"

#define KRAKEN_METADATA_KEY_MONITOR               "monitor"
#define KRAKEN_METADATA_KEY_DESKTOP_GRID_HORIZONTAL  "desktop-horizontal"
#define KRAKEN_METADATA_KEY_SHOW_THUMBNAILS "show-thumbnails"
#define KRAKEN_METADATA_KEY_DESKTOP_GRID_ADJUST      "desktop-grid-adjust"

#define KRAKEN_METADATA_KEY_PINNED                   "pinned-to-top"
#define KRAKEN_METADATA_KEY_FAVORITE                 "xapp-favorite"
#define KRAKEN_METADATA_KEY_FAVORITE_AVAILABLE     "xapp-favorite-available"

guint kraken_metadata_get_id (const char *metadata);

#endif /* KRAKEN_METADATA_H */
