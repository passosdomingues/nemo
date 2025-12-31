/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   kraken-search-directory-file.c: Subclass of KrakenFile to help implement the
   searches
 
   Copyright (C) 2005 Novell, Inc.
  
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
  
   Author: Anders Carlsson <andersca@imendio.com>
*/

#include <config.h>
#include "kraken-search-directory-file.h"

#include "kraken-directory-notify.h"
#include "kraken-directory-private.h"
#include "kraken-file-attributes.h"
#include "kraken-file-private.h"
#include "kraken-file-utilities.h"
#include <eel/eel-glib-extensions.h>
#include "kraken-search-directory.h"
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <string.h>

struct KrakenSearchDirectoryFileDetails {
	KrakenSearchDirectory *search_directory;
};

G_DEFINE_TYPE(KrakenSearchDirectoryFile, kraken_search_directory_file, KRAKEN_TYPE_FILE);


static void
search_directory_file_monitor_add (KrakenFile *file,
				   gconstpointer client,
				   KrakenFileAttributes attributes)
{
	/* No need for monitoring, we always emit changed when files
	   are added/removed, and no other metadata changes */

	/* Update display name, in case this didn't happen yet */
	kraken_search_directory_file_update_display_name (KRAKEN_SEARCH_DIRECTORY_FILE (file));
}

static void
search_directory_file_monitor_remove (KrakenFile *file,
				      gconstpointer client)
{
	/* Do nothing here, we don't have any monitors */
}

static void
search_directory_file_call_when_ready (KrakenFile *file,
				       KrakenFileAttributes file_attributes,
				       KrakenFileCallback callback,
				       gpointer callback_data)

{
	/* Update display name, in case this didn't happen yet */
	kraken_search_directory_file_update_display_name (KRAKEN_SEARCH_DIRECTORY_FILE (file));
	
	/* All data for directory-as-file is always uptodate */
	(* callback) (file, callback_data);
}
 
static void
search_directory_file_cancel_call_when_ready (KrakenFile *file,
					       KrakenFileCallback callback,
					       gpointer callback_data)
{
	/* Do nothing here, we don't have any pending calls */
}

static gboolean
search_directory_file_check_if_ready (KrakenFile *file,
				      KrakenFileAttributes attributes)
{
	return TRUE;
}

static gboolean
search_directory_file_get_item_count (KrakenFile *file, 
				      guint *count,
				      gboolean *count_unreadable)
{
	GList *file_list;

	if (count) {
		file_list = kraken_directory_get_file_list (file->details->directory);

		*count = g_list_length (file_list);

		kraken_file_list_free (file_list);
	}

	return TRUE;
}

static KrakenRequestStatus
search_directory_file_get_deep_counts (KrakenFile *file,
				       guint *directory_count,
				       guint *file_count,
				       guint *unreadable_directory_count,
                       guint *hidden_count,
				       goffset *total_size)
{
	KrakenFile *dir_file;
	GList *file_list, *l;
	guint dirs, files;
	GFileType type;

	file_list = kraken_directory_get_file_list (file->details->directory);

	dirs = files = 0;
	for (l = file_list; l != NULL; l = l->next) {
		dir_file = KRAKEN_FILE (l->data);
		type = kraken_file_get_file_type (dir_file);
		if (type == G_FILE_TYPE_DIRECTORY) {
			dirs++;
		} else {
			files++;
		}
	}

	if (directory_count != NULL) {
		*directory_count = dirs;
	}
	if (file_count != NULL) {
		*file_count = files;
	}
	if (unreadable_directory_count != NULL) {
		*unreadable_directory_count = 0;
	}
	if (total_size != NULL) {
		/* FIXME: Maybe we want to calculate this? */
		*total_size = 0;
	}
    if (hidden_count != NULL) {
        *hidden_count = 0;
    }

	kraken_file_list_free (file_list);
	
	return KRAKEN_REQUEST_DONE;
}

static char *
search_directory_file_get_where_string (KrakenFile *file)
{
	return g_strdup (_("Search"));
}

static gboolean
search_directory_get_date (KrakenFile     *file,
                           KrakenDateType  date_type,
                           time_t       *date)
{
    return FALSE;
}

void
kraken_search_directory_file_update_display_name (KrakenSearchDirectoryFile *search_file)
{
	KrakenFile *file;
	KrakenSearchDirectory *search_dir;
	KrakenQuery *query;
	char *display_name;
	gboolean changed;

	
	display_name = NULL;
	file = KRAKEN_FILE (search_file);
	if (file->details->directory) {
		search_dir = KRAKEN_SEARCH_DIRECTORY (file->details->directory);
		query = kraken_search_directory_get_query (search_dir);
	
		if (query != NULL) {
			display_name = kraken_query_to_readable_string (query);
			g_object_unref (query);
		} 
	}

	if (display_name == NULL) {
		display_name = g_strdup (_("Search"));
	}

	changed = kraken_file_set_display_name (file, display_name, NULL, TRUE);
	if (changed) {
		kraken_file_emit_changed (file);
	}

	g_free (display_name);
}

static void
kraken_search_directory_file_init (KrakenSearchDirectoryFile *search_file)
{
	KrakenFile *file;

	file = KRAKEN_FILE (search_file);

	file->details->got_file_info = TRUE;
	file->details->mime_type = g_ref_string_new_intern ("x-directory/normal");
	file->details->type = G_FILE_TYPE_DIRECTORY;
	file->details->size = 0;

	file->details->file_info_is_up_to_date = TRUE;

	file->details->custom_icon = NULL;
	file->details->activation_uri = NULL;
	file->details->got_link_info = TRUE;
	file->details->link_info_is_up_to_date = TRUE;

	file->details->directory_count = 0;
	file->details->got_directory_count = TRUE;
	file->details->directory_count_is_up_to_date = TRUE;

	kraken_file_set_display_name (file, _("Search"), NULL, TRUE);
}

static void
kraken_search_directory_file_class_init (KrakenSearchDirectoryFileClass *klass)
{
	KrakenFileClass *file_class;

	file_class = KRAKEN_FILE_CLASS (klass);

	file_class->default_file_type = G_FILE_TYPE_DIRECTORY;

	file_class->monitor_add = search_directory_file_monitor_add;
	file_class->monitor_remove = search_directory_file_monitor_remove;
	file_class->call_when_ready = search_directory_file_call_when_ready;
	file_class->cancel_call_when_ready = search_directory_file_cancel_call_when_ready;
	file_class->check_if_ready = search_directory_file_check_if_ready;
	file_class->get_item_count = search_directory_file_get_item_count;
	file_class->get_deep_counts = search_directory_file_get_deep_counts;
	file_class->get_where_string = search_directory_file_get_where_string;
    file_class->get_date = search_directory_get_date;
}
