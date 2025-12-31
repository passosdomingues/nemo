/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-*/

/* kraken-metadata.c - metadata utils
 *
 * Copyright (C) 2009 Red Hatl, Inc.
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

#include <config.h>
#include "kraken-metadata.h"
#include <glib.h>

static char *used_metadata_names[] = {
  (char *)KRAKEN_METADATA_KEY_DEFAULT_VIEW,
  (char *)KRAKEN_METADATA_KEY_LOCATION_BACKGROUND_COLOR,
  (char *)KRAKEN_METADATA_KEY_LOCATION_BACKGROUND_IMAGE,
  (char *)KRAKEN_METADATA_KEY_ICON_VIEW_ZOOM_LEVEL,
  (char *)KRAKEN_METADATA_KEY_ICON_VIEW_AUTO_LAYOUT,
  (char *)KRAKEN_METADATA_KEY_ICON_VIEW_SORT_BY,
  (char *)KRAKEN_METADATA_KEY_ICON_VIEW_SORT_REVERSED,
  (char *)KRAKEN_METADATA_KEY_ICON_VIEW_KEEP_ALIGNED,
  (char *)KRAKEN_METADATA_KEY_ICON_VIEW_LAYOUT_TIMESTAMP,
  (char *)KRAKEN_METADATA_KEY_LIST_VIEW_ZOOM_LEVEL,
  (char *)KRAKEN_METADATA_KEY_LIST_VIEW_SORT_COLUMN,
  (char *)KRAKEN_METADATA_KEY_LIST_VIEW_SORT_REVERSED,
  (char *)KRAKEN_METADATA_KEY_LIST_VIEW_VISIBLE_COLUMNS,
  (char *)KRAKEN_METADATA_KEY_LIST_VIEW_COLUMN_ORDER,
  (char *)KRAKEN_METADATA_KEY_COMPACT_VIEW_ZOOM_LEVEL,
  (char *)KRAKEN_METADATA_KEY_WINDOW_GEOMETRY,
  (char *)KRAKEN_METADATA_KEY_WINDOW_SCROLL_POSITION,
  (char *)KRAKEN_METADATA_KEY_WINDOW_SHOW_HIDDEN_FILES,
  (char *)KRAKEN_METADATA_KEY_WINDOW_MAXIMIZED,
  (char *)KRAKEN_METADATA_KEY_WINDOW_STICKY,
  (char *)KRAKEN_METADATA_KEY_WINDOW_KEEP_ABOVE,
  (char *)KRAKEN_METADATA_KEY_SIDEBAR_BACKGROUND_COLOR,
  (char *)KRAKEN_METADATA_KEY_SIDEBAR_BACKGROUND_IMAGE,
  (char *)KRAKEN_METADATA_KEY_SIDEBAR_BUTTONS,
  (char *)KRAKEN_METADATA_KEY_ANNOTATION,
  (char *)KRAKEN_METADATA_KEY_ICON_POSITION,
  (char *)KRAKEN_METADATA_KEY_ICON_POSITION_TIMESTAMP,
  (char *)KRAKEN_METADATA_KEY_ICON_SCALE,
  (char *)KRAKEN_METADATA_KEY_CUSTOM_ICON,
  (char *)KRAKEN_METADATA_KEY_CUSTOM_ICON_NAME,
  (char *)KRAKEN_METADATA_KEY_EMBLEMS,
  (char *)KRAKEN_METADATA_KEY_MONITOR,
  (char *)KRAKEN_METADATA_KEY_DESKTOP_GRID_HORIZONTAL,
  (char *)KRAKEN_METADATA_KEY_SHOW_THUMBNAILS,
  (char *)KRAKEN_METADATA_KEY_DESKTOP_GRID_ADJUST,
  (char *)KRAKEN_METADATA_KEY_PINNED,
  (char *)KRAKEN_METADATA_KEY_FAVORITE,
  (char *)KRAKEN_METADATA_KEY_FAVORITE_AVAILABLE,
  NULL
};

guint
kraken_metadata_get_id (const char *metadata)
{
  static GHashTable *hash;
  int i;

  if (hash == NULL)
    {
      hash = g_hash_table_new (g_str_hash, g_str_equal);
      for (i = 0; used_metadata_names[i] != NULL; i++)
	g_hash_table_insert (hash,
			     used_metadata_names[i],
			     GINT_TO_POINTER (i + 1));
    }

  return GPOINTER_TO_INT (g_hash_table_lookup (hash, metadata));
}
