/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* fm-empty-view.c - implementation of empty view of directory.

   Copyright (C) 2006 Free Software Foundation, Inc.
   
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

   Authors: Christian Neumair <chris@gnome-de.org>
*/

#include <config.h>

#include "kraken-empty-view.h"

#include "kraken-view.h"
#include "kraken-view-factory.h"

#include <string.h>
#include <libkraken-private/kraken-file-utilities.h>
#include <eel/eel-vfs-extensions.h>

struct KrakenEmptyViewDetails {
	int number_of_files;
};

static GList *kraken_empty_view_get_selection                   (KrakenView   *view);
static GList *kraken_empty_view_get_selection_for_file_transfer (KrakenView   *view);
static void   kraken_empty_view_scroll_to_file                  (KrakenView      *view,
								   const char        *uri);

G_DEFINE_TYPE (KrakenEmptyView, kraken_empty_view, KRAKEN_TYPE_VIEW)

static void
kraken_empty_view_add_file (KrakenView *view, KrakenFile *file, KrakenDirectory *directory)
{
	static GTimer *timer = NULL;
	static gdouble cumu = 0, elaps;
	KRAKEN_EMPTY_VIEW (view)->details->number_of_files++;
	GdkPixbuf *icon;

	if (!timer) timer = g_timer_new ();

	g_timer_start (timer);
	icon = kraken_file_get_icon_pixbuf (file, kraken_get_icon_size_for_zoom_level (KRAKEN_ZOOM_LEVEL_STANDARD), TRUE, 0, KRAKEN_FILE_ICON_FLAGS_NONE);

	elaps = g_timer_elapsed (timer, NULL);
	g_timer_stop (timer);

	g_object_unref (icon);
	
	cumu += elaps;
	g_message ("entire loading: %.3f, cumulative %.3f", elaps, cumu);
}


static void
kraken_empty_view_begin_loading (KrakenView *view)
{
}

static void
kraken_empty_view_clear (KrakenView *view)
{
}


static void
kraken_empty_view_file_changed (KrakenView *view, KrakenFile *file, KrakenDirectory *directory)
{
}

static GList *
kraken_empty_view_get_selection (KrakenView *view)
{
	return NULL;
}


static GList *
kraken_empty_view_get_selection_for_file_transfer (KrakenView *view)
{
	return NULL;
}

static guint
kraken_empty_view_get_item_count (KrakenView *view)
{
	return KRAKEN_EMPTY_VIEW (view)->details->number_of_files;
}

static gboolean
kraken_empty_view_is_empty (KrakenView *view)
{
	return KRAKEN_EMPTY_VIEW (view)->details->number_of_files == 0;
}

static void
kraken_empty_view_end_file_changes (KrakenView *view)
{
}

static void
kraken_empty_view_remove_file (KrakenView *view, KrakenFile *file, KrakenDirectory *directory)
{
	KRAKEN_EMPTY_VIEW (view)->details->number_of_files--;
	g_assert (KRAKEN_EMPTY_VIEW (view)->details->number_of_files >= 0);
}

static void
kraken_empty_view_set_selection (KrakenView *view, GList *selection)
{
	kraken_view_notify_selection_changed (view);
}

static void
kraken_empty_view_select_all (KrakenView *view)
{
}

static void
kraken_empty_view_reveal_selection (KrakenView *view)
{
}

static void
kraken_empty_view_merge_menus (KrakenView *view)
{
	KRAKEN_VIEW_CLASS (kraken_empty_view_parent_class)->merge_menus (view);
}

static void
kraken_empty_view_update_menus (KrakenView *view)
{
	KRAKEN_VIEW_CLASS (kraken_empty_view_parent_class)->update_menus (view);
}

/* Reset sort criteria and zoom level to match defaults */
static void
kraken_empty_view_reset_to_defaults (KrakenView *view)
{
}

static void
kraken_empty_view_bump_zoom_level (KrakenView *view, int zoom_increment)
{
}

static KrakenZoomLevel
kraken_empty_view_get_zoom_level (KrakenView *view)
{
	return KRAKEN_ZOOM_LEVEL_STANDARD;
}

static void
kraken_empty_view_zoom_to_level (KrakenView *view,
			    KrakenZoomLevel zoom_level)
{
}

static void
kraken_empty_view_restore_default_zoom_level (KrakenView *view)
{
}

static gboolean 
kraken_empty_view_can_zoom_in (KrakenView *view) 
{
	return FALSE;
}

static gboolean 
kraken_empty_view_can_zoom_out (KrakenView *view) 
{
	return FALSE;
}

static void
kraken_empty_view_start_renaming_file (KrakenView *view,
				  KrakenFile *file,
				  gboolean select_all)
{
}

static void
kraken_empty_view_click_policy_changed (KrakenView *directory_view)
{
}


static int
kraken_empty_view_compare_files (KrakenView *view, KrakenFile *file1, KrakenFile *file2)
{
	if (file1 < file2) {
		return -1;
	}

	if (file1 > file2) {
		return +1;
	}

	return 0;
}

static gboolean
kraken_empty_view_using_manual_layout (KrakenView *view)
{
	return FALSE;
}

static void
kraken_empty_view_end_loading (KrakenView *view,
			   gboolean all_files_seen)
{
}

static char *
kraken_empty_view_get_first_visible_file (KrakenView *view)
{
	return NULL;
}

static void
kraken_empty_view_scroll_to_file (KrakenView *view,
			      const char *uri)
{
}

static void
kraken_empty_view_sort_directories_first_changed (KrakenView *view)
{
}

static const char *
kraken_empty_view_get_id (KrakenView *view)
{
	return KRAKEN_EMPTY_VIEW_ID;
}

static void
kraken_empty_view_class_init (KrakenEmptyViewClass *class)
{
	KrakenViewClass *kraken_view_class;

	g_type_class_add_private (class, sizeof (KrakenEmptyViewDetails));

	kraken_view_class = KRAKEN_VIEW_CLASS (class);

	kraken_view_class->add_file = kraken_empty_view_add_file;
	kraken_view_class->begin_loading = kraken_empty_view_begin_loading;
	kraken_view_class->bump_zoom_level = kraken_empty_view_bump_zoom_level;
	kraken_view_class->can_zoom_in = kraken_empty_view_can_zoom_in;
	kraken_view_class->can_zoom_out = kraken_empty_view_can_zoom_out;
        kraken_view_class->click_policy_changed = kraken_empty_view_click_policy_changed;
	kraken_view_class->clear = kraken_empty_view_clear;
	kraken_view_class->file_changed = kraken_empty_view_file_changed;
	kraken_view_class->get_selection = kraken_empty_view_get_selection;
	kraken_view_class->get_selection_for_file_transfer = kraken_empty_view_get_selection_for_file_transfer;
	kraken_view_class->get_item_count = kraken_empty_view_get_item_count;
	kraken_view_class->is_empty = kraken_empty_view_is_empty;
	kraken_view_class->remove_file = kraken_empty_view_remove_file;
	kraken_view_class->merge_menus = kraken_empty_view_merge_menus;
	kraken_view_class->update_menus = kraken_empty_view_update_menus;
	kraken_view_class->reset_to_defaults = kraken_empty_view_reset_to_defaults;
	kraken_view_class->restore_default_zoom_level = kraken_empty_view_restore_default_zoom_level;
	kraken_view_class->reveal_selection = kraken_empty_view_reveal_selection;
	kraken_view_class->select_all = kraken_empty_view_select_all;
	kraken_view_class->set_selection = kraken_empty_view_set_selection;
	kraken_view_class->compare_files = kraken_empty_view_compare_files;
	kraken_view_class->sort_directories_first_changed = kraken_empty_view_sort_directories_first_changed;
	kraken_view_class->start_renaming_file = kraken_empty_view_start_renaming_file;
	kraken_view_class->get_zoom_level = kraken_empty_view_get_zoom_level;
	kraken_view_class->zoom_to_level = kraken_empty_view_zoom_to_level;
	kraken_view_class->end_file_changes = kraken_empty_view_end_file_changes;
	kraken_view_class->using_manual_layout = kraken_empty_view_using_manual_layout;
	kraken_view_class->end_loading = kraken_empty_view_end_loading;
	kraken_view_class->get_view_id = kraken_empty_view_get_id;
	kraken_view_class->get_first_visible_file = kraken_empty_view_get_first_visible_file;
	kraken_view_class->scroll_to_file = kraken_empty_view_scroll_to_file;
}

static void
kraken_empty_view_init (KrakenEmptyView *empty_view)
{
	empty_view->details = G_TYPE_INSTANCE_GET_PRIVATE (empty_view, KRAKEN_TYPE_EMPTY_VIEW,
							   KrakenEmptyViewDetails);
}

static KrakenView *
kraken_empty_view_create (KrakenWindowSlot *slot)
{
	KrakenEmptyView *view;

	g_assert (KRAKEN_IS_WINDOW_SLOT (slot));

	view = g_object_new (KRAKEN_TYPE_EMPTY_VIEW,
			     "window-slot", slot,
			     NULL);

	return KRAKEN_VIEW (view);
}

static gboolean
kraken_empty_view_supports_uri (const char *uri,
				  GFileType file_type,
				  const char *mime_type)
{
	if (file_type == G_FILE_TYPE_DIRECTORY) {
		return TRUE;
	}
	if (strcmp (mime_type, KRAKEN_SAVED_SEARCH_MIMETYPE) == 0){
		return TRUE;
	}
	if (g_str_has_prefix (uri, "trash:")) {
		return TRUE;
	}
	if (g_str_has_prefix (uri, EEL_SEARCH_URI)) {
		return TRUE;
	}

	return FALSE;
}

static KrakenViewInfo kraken_empty_view = {
	KRAKEN_EMPTY_VIEW_ID,
	"Empty",
	"Empty View",
	"_Empty View",
	"The empty view encountered an error.",
	"Display this location with the empty view.",
	kraken_empty_view_create,
	kraken_empty_view_supports_uri
};

void
kraken_empty_view_register (void)
{
	kraken_empty_view.id = kraken_empty_view.id;
	kraken_empty_view.view_combo_label = kraken_empty_view.view_combo_label;
	kraken_empty_view.view_menu_label_with_mnemonic = kraken_empty_view.view_menu_label_with_mnemonic;
	kraken_empty_view.error_label = kraken_empty_view.error_label;
	kraken_empty_view.display_location_label = kraken_empty_view.display_location_label;

	kraken_view_factory_register (&kraken_empty_view);
}
