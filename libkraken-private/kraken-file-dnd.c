/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* kraken-file-drag.c - Drag & drop handling code that operated on 
   KrakenFile objects.

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

   Authors: Pavel Cisler <pavel@eazel.com>,
*/

#include <config.h>
#include "kraken-file-dnd.h"
#include "kraken-desktop-icon-file.h"

#include "kraken-dnd.h"
#include "kraken-directory.h"
#include "kraken-file-utilities.h"
#include <string.h>

static gboolean
kraken_drag_can_accept_files (KrakenFile *drop_target_item)
{
	KrakenDirectory *directory;

	if (kraken_file_is_directory (drop_target_item)) {
		gboolean res;

		/* target is a directory, accept if editable */
		directory = kraken_directory_get_for_file (drop_target_item);
		res = kraken_directory_is_editable (directory);
		kraken_directory_unref (directory);
		return res;
	}
	
	if (KRAKEN_IS_DESKTOP_ICON_FILE (drop_target_item)) {
		return TRUE;
	}
	
	/* Launchers are an acceptable drop target */
	if (kraken_file_is_launcher (drop_target_item)) {
		return TRUE;
	}

	if (kraken_is_file_roller_installed () &&
	    kraken_file_is_archive (drop_target_item)) {
		return TRUE;
	}
	
	return FALSE;
}

gboolean
kraken_drag_can_accept_item (KrakenFile *drop_target_item,
			       const char *item_uri)
{
	if (kraken_file_matches_uri (drop_target_item, item_uri)) {
		/* can't accept itself */
		return FALSE;
	}

	return kraken_drag_can_accept_files (drop_target_item);
}
				       
gboolean
kraken_drag_can_accept_items (KrakenFile *drop_target_item,
				const GList *items)
{
	int max;

	if (drop_target_item == NULL)
		return FALSE;

	g_assert (KRAKEN_IS_FILE (drop_target_item));

	/* Iterate through selection checking if item will get accepted by the
	 * drop target. If more than 100 items selected, return an over-optimisic
	 * result
	 */
	for (max = 100; items != NULL && max >= 0; items = items->next, max--) {
		if (!kraken_drag_can_accept_item (drop_target_item, 
			((KrakenDragSelectionItem *)items->data)->uri)) {
			return FALSE;
		}
	}
	
	return TRUE;
}

gboolean
kraken_drag_can_accept_info (KrakenFile *drop_target_item,
			       KrakenIconDndTargetType drag_type,
			       const GList *items)
{
	switch (drag_type) {
		case KRAKEN_ICON_DND_GNOME_ICON_LIST:
			return kraken_drag_can_accept_items (drop_target_item, items);

		case KRAKEN_ICON_DND_URI_LIST:
		case KRAKEN_ICON_DND_NETSCAPE_URL:
		case KRAKEN_ICON_DND_TEXT:
			return kraken_drag_can_accept_files (drop_target_item);

		case KRAKEN_ICON_DND_XDNDDIRECTSAVE:
		case KRAKEN_ICON_DND_RAW:
			return kraken_drag_can_accept_files (drop_target_item); /* Check if we can accept files at this location */

		case KRAKEN_ICON_DND_ROOTWINDOW_DROP:
			return FALSE;

		default:
			g_assert_not_reached ();
			return FALSE;
	}
}

