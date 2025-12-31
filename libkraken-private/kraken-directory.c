/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   kraken-directory.c: Kraken directory model.
 
   Copyright (C) 1999, 2000, 2001 Eazel, Inc.
  
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
  
   Author: Darin Adler <darin@bentspoon.com>
*/

#include <config.h>
#include "kraken-directory-private.h"

#include "kraken-directory-notify.h"
#include "kraken-file-attributes.h"
#include "kraken-file-private.h"
#include "kraken-file-utilities.h"
#include "kraken-search-directory.h"
#include "kraken-global-preferences.h"
#include "kraken-lib-self-check-functions.h"
#include "kraken-metadata.h"
#include "kraken-desktop-directory.h"
#include "kraken-vfs-directory.h"
#include "kraken-tag-directory.h"
#include <eel/eel-glib-extensions.h>
#include <eel/eel-string.h>
#include <gtk/gtk.h>

enum {
	FILES_ADDED,
	FILES_CHANGED,
	DONE_LOADING,
	LOAD_ERROR,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

static GHashTable *directories;

static void               kraken_directory_finalize         (GObject                *object);
static KrakenDirectory *kraken_directory_new              (GFile                  *location);
static GList *            real_get_file_list                  (KrakenDirectory      *directory);
static gboolean		  real_is_editable                    (KrakenDirectory      *directory);
static void               set_directory_location              (KrakenDirectory      *directory,
							       GFile                  *location);

G_DEFINE_TYPE (KrakenDirectory, kraken_directory, G_TYPE_OBJECT);

static void
kraken_directory_class_init (KrakenDirectoryClass *klass)
{
	GObjectClass *object_class;

	object_class = G_OBJECT_CLASS (klass);
	
	object_class->finalize = kraken_directory_finalize;

	signals[FILES_ADDED] =
		g_signal_new ("files_added",
		              G_TYPE_FROM_CLASS (object_class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (KrakenDirectoryClass, files_added),
		              NULL, NULL,
		              g_cclosure_marshal_VOID__POINTER,
		              G_TYPE_NONE, 1, G_TYPE_POINTER);
	signals[FILES_CHANGED] =
		g_signal_new ("files_changed",
		              G_TYPE_FROM_CLASS (object_class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (KrakenDirectoryClass, files_changed),
		              NULL, NULL,
		              g_cclosure_marshal_VOID__POINTER,
		              G_TYPE_NONE, 1, G_TYPE_POINTER);
	signals[DONE_LOADING] =
		g_signal_new ("done_loading",
		              G_TYPE_FROM_CLASS (object_class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (KrakenDirectoryClass, done_loading),
		              NULL, NULL,
		              g_cclosure_marshal_VOID__VOID,
		              G_TYPE_NONE, 0);
	signals[LOAD_ERROR] =
		g_signal_new ("load_error",
		              G_TYPE_FROM_CLASS (object_class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (KrakenDirectoryClass, load_error),
		              NULL, NULL,
		              g_cclosure_marshal_VOID__POINTER,
		              G_TYPE_NONE, 1, G_TYPE_POINTER);

	klass->get_file_list = real_get_file_list;
	klass->is_editable = real_is_editable;

	g_type_class_add_private (klass, sizeof (KrakenDirectoryDetails));
}

static void
kraken_directory_init (KrakenDirectory *directory)
{
	directory->details = G_TYPE_INSTANCE_GET_PRIVATE ((directory), KRAKEN_TYPE_DIRECTORY, KrakenDirectoryDetails);
	directory->details->file_hash = g_hash_table_new (g_str_hash, g_str_equal);
	directory->details->high_priority_queue = kraken_file_queue_new ();
	directory->details->low_priority_queue = kraken_file_queue_new ();
	directory->details->extension_queue = kraken_file_queue_new ();
    directory->details->max_deferred_file_count = g_settings_get_int (kraken_preferences,
                                                                      KRAKEN_PREFERENCES_DEFERRED_ATTR_PRELOAD_LIMIT);
}

KrakenDirectory *
kraken_directory_ref (KrakenDirectory *directory)
{
	if (directory == NULL) {
		return directory;
	}

	g_return_val_if_fail (KRAKEN_IS_DIRECTORY (directory), NULL);

	g_object_ref (directory);
	return directory;
}

void
kraken_directory_unref (KrakenDirectory *directory)
{
	if (directory == NULL) {
		return;
	}

	g_return_if_fail (KRAKEN_IS_DIRECTORY (directory));

	g_object_unref (directory);
}

static void
kraken_directory_finalize (GObject *object)
{
	KrakenDirectory *directory;

	directory = KRAKEN_DIRECTORY (object);

	g_hash_table_remove (directories, directory->details->location);

	kraken_directory_cancel (directory);
	g_assert (directory->details->count_in_progress == NULL);

	if (directory->details->monitor_list != NULL) {
		g_warning ("destroying a KrakenDirectory while it's being monitored");
		g_list_free_full (directory->details->monitor_list, g_free);
	}

	if (directory->details->monitor != NULL) {
		kraken_monitor_cancel (directory->details->monitor);
	}

	if (directory->details->dequeue_pending_idle_id != 0) {
		g_source_remove (directory->details->dequeue_pending_idle_id);
	}

	if (directory->details->call_ready_idle_id != 0) {
		g_source_remove (directory->details->call_ready_idle_id);
	}

	if (directory->details->location) {
		g_object_unref (directory->details->location);
	}

	g_assert (directory->details->file_list == NULL);
	g_hash_table_destroy (directory->details->file_hash);

	kraken_file_queue_destroy (directory->details->high_priority_queue);
	kraken_file_queue_destroy (directory->details->low_priority_queue);
	kraken_file_queue_destroy (directory->details->extension_queue);
	g_assert (directory->details->directory_load_in_progress == NULL);
	g_assert (directory->details->count_in_progress == NULL);
	g_assert (directory->details->dequeue_pending_idle_id == 0);
	g_list_free_full (directory->details->pending_file_info, g_object_unref);

	G_OBJECT_CLASS (kraken_directory_parent_class)->finalize (object);
}

static void
invalidate_one_count (gpointer key, gpointer value, gpointer user_data)
{
	KrakenDirectory *directory;

	g_assert (key != NULL);
	g_assert (KRAKEN_IS_DIRECTORY (value));
	g_assert (user_data == NULL);

	directory = KRAKEN_DIRECTORY (value);
	
	kraken_directory_invalidate_count_and_mime_list (directory);
}

static void
filtering_changed_callback (gpointer callback_data)
{
	g_assert (callback_data == NULL);

	/* Preference about which items to show has changed, so we
	 * can't trust any of our precomputed directory counts.
	 */
	g_hash_table_foreach (directories, invalidate_one_count, NULL);
}

void
emit_change_signals_for_all_files (KrakenDirectory *directory)
{
	GList *files;

	files = g_list_copy (directory->details->file_list);
	if (directory->details->as_file != NULL) {
		files = g_list_prepend (files, directory->details->as_file);
	}

	kraken_file_list_ref (files);
	kraken_directory_emit_change_signals (directory, files);

	kraken_file_list_free (files);
}

static void
collect_all_directories (gpointer key, gpointer value, gpointer callback_data)
{
	KrakenDirectory *directory;
	GList **dirs;

	directory = KRAKEN_DIRECTORY (value);
	dirs = callback_data;

	*dirs = g_list_prepend (*dirs, kraken_directory_ref (directory));
}

void
emit_change_signals_for_all_files_in_all_directories (void)
{
	GList *dirs, *l;
	KrakenDirectory *directory;

	dirs = NULL;
	g_hash_table_foreach (directories,
			      collect_all_directories,
			      &dirs);

	for (l = dirs; l != NULL; l = l->next) {
		directory = KRAKEN_DIRECTORY (l->data);
		emit_change_signals_for_all_files (directory);
		kraken_directory_unref (directory);
	}

	g_list_free (dirs);
}

static void
async_state_changed_one (gpointer key, gpointer value, gpointer user_data)
{
	KrakenDirectory *directory;

	g_assert (key != NULL);
	g_assert (KRAKEN_IS_DIRECTORY (value));
	g_assert (user_data == NULL);

	directory = KRAKEN_DIRECTORY (value);
	
	kraken_directory_async_state_changed (directory);
	emit_change_signals_for_all_files (directory);
}

static void
async_data_preference_changed_callback (gpointer callback_data)
{
	g_assert (callback_data == NULL);

	/* Preference involving fetched async data has changed, so
	 * we have to kick off refetching all async data, and tell
	 * each file that it (might have) changed.
	 */
	g_hash_table_foreach (directories, async_state_changed_one, NULL);
}

static void
add_preferences_callbacks (void)
{
	kraken_global_preferences_init ();

	g_signal_connect_swapped (kraken_preferences,
				  "changed::" KRAKEN_PREFERENCES_SHOW_HIDDEN_FILES,
				  G_CALLBACK(filtering_changed_callback),
				  NULL);
	g_signal_connect_swapped (kraken_preferences,
				  "changed::" KRAKEN_PREFERENCES_SHOW_DIRECTORY_ITEM_COUNTS,
				  G_CALLBACK (async_data_preference_changed_callback),
				  NULL);
	g_signal_connect_swapped (kraken_preferences,
				  "changed::" KRAKEN_PREFERENCES_DATE_FORMAT,
				  G_CALLBACK(async_data_preference_changed_callback),
				  NULL);
}

/**
 * kraken_directory_get_by_uri:
 * @uri: URI of directory to get.
 *
 * Get a directory given a uri.
 * Creates the appropriate subclass given the uri mappings.
 * Returns a referenced object, not a floating one. Unref when finished.
 * If two windows are viewing the same uri, the directory object is shared.
 */
KrakenDirectory *
kraken_directory_get_internal (GFile *location, gboolean create)
{
	KrakenDirectory *directory;
	
	/* Create the hash table first time through. */
	if (directories == NULL) {
		directories = g_hash_table_new (g_file_hash, (GCompareFunc) g_file_equal);
		add_preferences_callbacks ();
	}

	/* If the object is already in the hash table, look it up. */

	directory = g_hash_table_lookup (directories,
					 location);
	if (directory != NULL) {
		kraken_directory_ref (directory);
	} else if (create) {
		/* Create a new directory object instead. */
		directory = kraken_directory_new (location);
		if (directory == NULL) {
			return NULL;
		}

		/* Put it in the hash table. */
		g_hash_table_insert (directories,
				     directory->details->location,
				     directory);
	}

	return directory;
}

KrakenDirectory *
kraken_directory_get (GFile *location)
{
	if (location == NULL) {
    		return NULL;
	}

	return kraken_directory_get_internal (location, TRUE);
}

KrakenDirectory *
kraken_directory_get_existing (GFile *location)
{
	if (location == NULL) {
    		return NULL;
	}

	return kraken_directory_get_internal (location, FALSE);
}


KrakenDirectory *
kraken_directory_get_by_uri (const char *uri)
{
	KrakenDirectory *directory;
	GFile *location;

	if (uri == NULL) {
    		return NULL;
	}

	location = g_file_new_for_uri (uri);

	directory = kraken_directory_get_internal (location, TRUE);
	g_object_unref (location);
	return directory;
}

KrakenDirectory *
kraken_directory_get_for_file (KrakenFile *file)
{
	char *uri;
	KrakenDirectory *directory;

	g_return_val_if_fail (KRAKEN_IS_FILE (file), NULL);

	uri = kraken_file_get_uri (file);
	directory = kraken_directory_get_by_uri (uri);
	g_free (uri);
	return directory;
}

/* Returns a reffed KrakenFile object for this directory.
 */
KrakenFile *
kraken_directory_get_corresponding_file (KrakenDirectory *directory)
{
	KrakenFile *file;
	char *uri;

	file = kraken_directory_get_existing_corresponding_file (directory);
	if (file == NULL) {
		uri = kraken_directory_get_uri (directory);
		file = kraken_file_get_by_uri (uri);
		g_free (uri);
	}

	return file;
}

/* Returns a reffed KrakenFile object for this directory, but only if the
 * KrakenFile object has already been created.
 */
KrakenFile *
kraken_directory_get_existing_corresponding_file (KrakenDirectory *directory)
{
	KrakenFile *file;
	char *uri;
	
	file = directory->details->as_file;
	if (file != NULL) {
		kraken_file_ref (file);
		return file;
	}

	uri = kraken_directory_get_uri (directory);
	file = kraken_file_get_existing_by_uri (uri);
	g_free (uri);
	return file;
}

/* kraken_directory_get_name_for_self_as_new_file:
 * 
 * Get a name to display for the file representing this
 * directory. This is called only when there's no VFS
 * directory for this KrakenDirectory.
 */
char *
kraken_directory_get_name_for_self_as_new_file (KrakenDirectory *directory)
{
	char *directory_uri;
	char *name, *colon;
	
	directory_uri = kraken_directory_get_uri (directory);

	colon = strchr (directory_uri, ':');
	if (colon == NULL || colon == directory_uri) {
		name = g_strdup (directory_uri);
	} else {
		name = g_strndup (directory_uri, colon - directory_uri);
	}
	g_free (directory_uri);
	
	return name;
}

char *
kraken_directory_get_uri (KrakenDirectory *directory)
{
	g_return_val_if_fail (KRAKEN_IS_DIRECTORY (directory), NULL);

	return g_file_get_uri (directory->details->location);
}

GFile *
kraken_directory_get_location (KrakenDirectory  *directory)
{
	g_return_val_if_fail (KRAKEN_IS_DIRECTORY (directory), NULL);

	return g_object_ref (directory->details->location);
}

static KrakenDirectory *
kraken_directory_new (GFile *location)
{
	KrakenDirectory *directory;
	char *uri;

	uri = g_file_get_uri (location);
	
	if (eel_uri_is_desktop (uri)) {
		directory = KRAKEN_DIRECTORY (g_object_new (KRAKEN_TYPE_DESKTOP_DIRECTORY, NULL));
	} else if (eel_uri_is_search (uri)) {
		directory = KRAKEN_DIRECTORY (g_object_new (KRAKEN_TYPE_SEARCH_DIRECTORY, NULL));
	} else if (g_str_has_prefix (uri, "kraken://tags")) {
		directory = KRAKEN_DIRECTORY (g_object_new (KRAKEN_TYPE_TAG_DIRECTORY, NULL));
	} else {
		directory = KRAKEN_DIRECTORY (g_object_new (KRAKEN_TYPE_VFS_DIRECTORY, NULL));
	}

	set_directory_location (directory, location);

	g_free (uri);
	
	return directory;
}

gboolean
kraken_directory_is_local (KrakenDirectory *directory)
{
	g_return_val_if_fail (KRAKEN_IS_DIRECTORY (directory), FALSE);
	
	if (directory->details->location == NULL) {
		return TRUE;
	}

	return kraken_directory_is_in_trash (directory) ||
	       g_file_is_native (directory->details->location);
}

gboolean
kraken_directory_is_in_trash (KrakenDirectory *directory)
{
	g_assert (KRAKEN_IS_DIRECTORY (directory));
	
	if (directory->details->location == NULL) {
		return FALSE;
	}

	return g_file_has_uri_scheme (directory->details->location, "trash");
}

gboolean
kraken_directory_is_in_recent (KrakenDirectory *directory)
{
   g_assert (KRAKEN_IS_DIRECTORY (directory));

   if (directory->details->location == NULL) {
       return FALSE;
   }

   return g_file_has_uri_scheme (directory->details->location, "recent");
}

gboolean
kraken_directory_is_in_search (KrakenDirectory *directory)
{
   g_assert (KRAKEN_IS_DIRECTORY (directory));

   if (directory->details->location == NULL) {
       return FALSE;
   }

   return g_file_has_uri_scheme (directory->details->location, "x-kraken-search");
}

gboolean
kraken_directory_is_in_tags (KrakenDirectory *directory)
{
   g_assert (KRAKEN_IS_DIRECTORY (directory));

   if (directory->details->location == NULL) {
       return FALSE;
   }

   return g_file_has_uri_scheme (directory->details->location, "kraken");
}

gboolean
kraken_directory_is_in_favorites (KrakenDirectory *directory)
{
   g_assert (KRAKEN_IS_DIRECTORY (directory));

   if (directory->details->location == NULL) {
       return FALSE;
   }

   return g_file_has_uri_scheme (directory->details->location, "favorites");
}

gboolean
kraken_directory_is_in_admin (KrakenDirectory *directory)
{
    g_assert (KRAKEN_IS_DIRECTORY (directory));

    if (directory->details->location == NULL)
    {
        return FALSE;
    }

    return g_file_has_uri_scheme (directory->details->location, "admin");
}

gboolean
kraken_directory_are_all_files_seen (KrakenDirectory *directory)
{
	g_return_val_if_fail (KRAKEN_IS_DIRECTORY (directory), FALSE);

	return KRAKEN_DIRECTORY_CLASS (G_OBJECT_GET_CLASS (directory))->are_all_files_seen (directory);
}

static void
add_to_hash_table (KrakenDirectory *directory, KrakenFile *file, GList *node)
{
	const char *name;

	name = file->details->name;

	g_assert (node != NULL);
	g_assert (g_hash_table_lookup (directory->details->file_hash,
				       name) == NULL);
	g_hash_table_insert (directory->details->file_hash, (char *) name, node);
}

static GList *
extract_from_hash_table (KrakenDirectory *directory, KrakenFile *file)
{
	const char *name;
	GList *node;

	name = file->details->name;
	if (name == NULL) {
		return NULL;
	}

	/* Find the list node in the hash table. */
	node = g_hash_table_lookup (directory->details->file_hash, name);
	g_hash_table_remove (directory->details->file_hash, name);

	return node;
}

void
kraken_directory_add_file (KrakenDirectory *directory, KrakenFile *file)
{
	GList *node;
	gboolean add_to_work_queue;

	g_assert (KRAKEN_IS_DIRECTORY (directory));
	g_assert (KRAKEN_IS_FILE (file));
	g_assert (file->details->name != NULL);

	/* Add to list. */
	node = g_list_prepend (directory->details->file_list, file);
	directory->details->file_list = node;

	/* Add to hash table. */
	add_to_hash_table (directory, file, node);

	directory->details->confirmed_file_count++;

    if (directory->details->early_load_file_count++ < directory->details->max_deferred_file_count) {
        file->details->load_deferred_attrs = KRAKEN_FILE_LOAD_DEFERRED_ATTRS_PRELOAD;
    }

	add_to_work_queue = FALSE;
	if (kraken_directory_is_file_list_monitored (directory)) {
		/* Ref if we are monitoring, since monitoring owns the file list. */
		kraken_file_ref (file);
		add_to_work_queue = TRUE;
	} else if (kraken_directory_has_active_request_for_file (directory, file)) {
		/* We're waiting for the file in a call_when_ready. Make sure
		   we add the file to the work queue so that said waiter won't
		   wait forever for e.g. all files in the directory to be done */
		add_to_work_queue = TRUE;
	}
	
	if (add_to_work_queue) {
		kraken_directory_add_file_to_work_queue (directory, file);
	}
}

void
kraken_directory_remove_file (KrakenDirectory *directory, KrakenFile *file)
{
	GList *node;

	g_assert (KRAKEN_IS_DIRECTORY (directory));
	g_assert (KRAKEN_IS_FILE (file));
	g_assert (file->details->name != NULL);

	/* Find the list node in the hash table. */
	node = extract_from_hash_table (directory, file);
	g_assert (node != NULL);
	g_assert (node->data == file);

	/* Remove the item from the list. */
	directory->details->file_list = g_list_remove_link
		(directory->details->file_list, node);
	g_list_free_1 (node);

	kraken_directory_remove_file_from_work_queue (directory, file);

	if (!file->details->unconfirmed) {
		directory->details->confirmed_file_count--;
	}

	/* Unref if we are monitoring. */
	if (kraken_directory_is_file_list_monitored (directory)) {
		kraken_file_unref (file);
	}
}

GList *
kraken_directory_begin_file_name_change (KrakenDirectory *directory,
					   KrakenFile *file)
{
	/* Find the list node in the hash table. */
	return extract_from_hash_table (directory, file);
}

void
kraken_directory_end_file_name_change (KrakenDirectory *directory,
					 KrakenFile *file,
					 GList *node)
{
	/* Add the list node to the hash table. */
	if (node != NULL) {
		add_to_hash_table (directory, file, node);
	}
}

KrakenFile *
kraken_directory_find_file_by_name (KrakenDirectory *directory,
				      const char *name)
{
	GList *node;

	g_return_val_if_fail (KRAKEN_IS_DIRECTORY (directory), NULL);
	g_return_val_if_fail (name != NULL, NULL);

	node = g_hash_table_lookup (directory->details->file_hash,
				    name);
	return node == NULL ? NULL : KRAKEN_FILE (node->data);
}

/* "." for the directory-as-file, otherwise the filename */
KrakenFile *
kraken_directory_find_file_by_internal_filename (KrakenDirectory *directory,
						   const char *internal_filename)
{
	KrakenFile *result;

	if (g_strcmp0 (internal_filename, ".") == 0) {
		result = kraken_directory_get_existing_corresponding_file (directory);
		if (result != NULL) {
			kraken_file_unref (result);
		}
	} else {
		result = kraken_directory_find_file_by_name (directory, internal_filename);
	}

	return result;
}

void
kraken_directory_emit_files_added (KrakenDirectory *directory,
				     GList *added_files)
{
	if (added_files != NULL) {
		g_signal_emit (directory,
				 signals[FILES_ADDED], 0,
				 added_files);
	}
}

void
kraken_directory_emit_files_changed (KrakenDirectory *directory,
				       GList *changed_files)
{
	if (changed_files != NULL) {
		g_signal_emit (directory,
				 signals[FILES_CHANGED], 0,
				 changed_files);
	}
}

void
kraken_directory_emit_change_signals (KrakenDirectory *directory,
					     GList *changed_files)
{
	GList *p;

	for (p = changed_files; p != NULL; p = p->next) {
		kraken_file_emit_changed (p->data);
	}
	kraken_directory_emit_files_changed (directory, changed_files);
}

void
kraken_directory_emit_done_loading (KrakenDirectory *directory)
{
    directory->details->early_load_file_count = 0;

	g_signal_emit (directory,
			 signals[DONE_LOADING], 0);
}

void
kraken_directory_emit_load_error (KrakenDirectory *directory,
				    GError *error)
{
	g_signal_emit (directory,
			 signals[LOAD_ERROR], 0,
			 error);
}

/* Return a directory object for this one's parent. */
static KrakenDirectory *
get_parent_directory (GFile *location)
{
	KrakenDirectory *directory;
	GFile *parent;

	parent = g_file_get_parent (location);
	if (parent) {
		directory = kraken_directory_get_internal (parent, TRUE);
		g_object_unref (parent);
		return directory;
	}
	return NULL;
}

/* If a directory object exists for this one's parent, then
 * return it, otherwise return NULL.
 */
static KrakenDirectory *
get_parent_directory_if_exists (GFile *location)
{
	KrakenDirectory *directory;
	GFile *parent;

	parent = g_file_get_parent (location);
	if (parent) {
		directory = kraken_directory_get_internal (parent, FALSE);
		g_object_unref (parent);
		return directory;
	}
	return NULL;
}

static void
hash_table_list_prepend (GHashTable *table, gconstpointer key, gpointer data)
{
	GList *list;

	list = g_hash_table_lookup (table, key);
	list = g_list_prepend (list, data);
	g_hash_table_insert (table, (gpointer) key, list);
}

static void
call_files_added_free_list (gpointer key, gpointer value, gpointer user_data)
{
	g_assert (KRAKEN_IS_DIRECTORY (key));
	g_assert (value != NULL);
	g_assert (user_data == NULL);

	g_signal_emit (key,
			 signals[FILES_ADDED], 0,
			 value);
	g_list_free (value);
}

static void
call_files_changed_common (KrakenDirectory *directory, GList *file_list)
{
	GList *node;
	KrakenFile *file;

	for (node = file_list; node != NULL; node = node->next) {
		file = node->data;
		if (file->details->directory == directory) {
			kraken_directory_add_file_to_work_queue (directory, 
								   file);
		}
	}
	kraken_directory_async_state_changed (directory);
	kraken_directory_emit_change_signals (directory, file_list);
}

static void
call_files_changed_free_list (gpointer key, gpointer value, gpointer user_data)
{
	g_assert (value != NULL);
	g_assert (user_data == NULL);

	call_files_changed_common (KRAKEN_DIRECTORY (key), value);
	g_list_free (value);
}

static void
call_files_changed_unref_free_list (gpointer key, gpointer value, gpointer user_data)
{
	g_assert (value != NULL);
	g_assert (user_data == NULL);

	call_files_changed_common (KRAKEN_DIRECTORY (key), value);
	kraken_file_list_free (value);
}

static void
call_get_file_info_free_list (gpointer key, gpointer value, gpointer user_data)
{
	KrakenDirectory *directory;
	GList *files;
	
	g_assert (KRAKEN_IS_DIRECTORY (key));
	g_assert (value != NULL);
	g_assert (user_data == NULL);

	directory = key;
	files = value;
	
	kraken_directory_get_info_for_new_files (directory, files);
	g_list_foreach (files, (GFunc) g_object_unref, NULL);
	g_list_free (files);
}

static void
invalidate_count_and_unref (gpointer key, gpointer value, gpointer user_data)
{
	g_assert (KRAKEN_IS_DIRECTORY (key));
	g_assert (value == key);
	g_assert (user_data == NULL);

	kraken_directory_invalidate_count_and_mime_list (key);
	kraken_directory_unref (key);
}

static void
collect_parent_directories (GHashTable *hash_table, KrakenDirectory *directory)
{
	g_assert (hash_table != NULL);
	g_assert (KRAKEN_IS_DIRECTORY (directory));

	if (g_hash_table_lookup (hash_table, directory) == NULL) {
		kraken_directory_ref (directory);
		g_hash_table_insert  (hash_table, directory, directory);
	}
}

void
kraken_directory_notify_files_added (GList *files)
{
	GHashTable *added_lists;
	GList *p;
	KrakenDirectory *directory;
	GHashTable *parent_directories;
	KrakenFile *file;
	GFile *location, *parent;

	/* Make a list of added files in each directory. */
	added_lists = g_hash_table_new (NULL, NULL);

	/* Make a list of parent directories that will need their counts updated. */
	parent_directories = g_hash_table_new (NULL, NULL);

	for (p = files; p != NULL; p = p->next) {
		location = p->data;

		/* See if the directory is already known. */
		directory = get_parent_directory_if_exists (location);
		if (directory == NULL) {
			/* In case the directory is not being
			 * monitored, but the corresponding file is,
			 * we must invalidate it's item count.
			 */


			file = NULL;
			parent = g_file_get_parent (location);
			if (parent) {
				file = kraken_file_get_existing (parent);
				g_object_unref (parent);
			}

			if (file != NULL) {
				kraken_file_invalidate_count_and_mime_list (file);
				kraken_file_unref (file);
			}

			continue;
		}

		collect_parent_directories (parent_directories, directory);

		/* If no one is monitoring files in the directory, nothing to do. */
		if (!kraken_directory_is_file_list_monitored (directory)) {
			kraken_directory_unref (directory);
			continue;
		}

        file = kraken_file_get_existing (location);
        /* We check is_added here, because the file could have been added
         * to the directory by a kraken_file_get() but not gotten
         * files_added emitted
         */
        if (file && file->details->is_added) {
            /* A file already exists, it was probably renamed.
             * If it was renamed this could be ignored, but
             * queue a change just in case */
            kraken_file_changed (file);
        } else {
            hash_table_list_prepend (added_lists,
                                     directory,
                                     g_object_ref (location));
        }

        kraken_file_unref (file);
        kraken_directory_unref (directory);
    }

	/* Now get file info for the new files. This creates KrakenFile
	 * objects for the new files, and sends out a files_added signal. 
	 */
	g_hash_table_foreach (added_lists, call_get_file_info_free_list, NULL);
	g_hash_table_destroy (added_lists);

	/* Invalidate count for each parent directory. */
	g_hash_table_foreach (parent_directories, invalidate_count_and_unref, NULL);
	g_hash_table_destroy (parent_directories);
}

static void
g_file_pair_free (GFilePair *pair)
{
	g_object_unref (pair->to);
	g_object_unref (pair->from);
	g_free (pair);
}

static GList *
uri_pairs_to_file_pairs (GList *uri_pairs)
{
	GList *l, *file_pair_list;
	GFilePair *file_pair;
	URIPair *uri_pair;
	
	file_pair_list = NULL;

	for (l = uri_pairs; l != NULL; l = l->next) {
		uri_pair = l->data;
		file_pair = g_new (GFilePair, 1);
		file_pair->from = g_file_new_for_uri (uri_pair->from_uri);
		file_pair->to = g_file_new_for_uri (uri_pair->to_uri);
		
		file_pair_list = g_list_prepend (file_pair_list, file_pair);
	}
	return g_list_reverse (file_pair_list);
}

void
kraken_directory_notify_files_added_by_uri (GList *uris)
{
	GList *files;

	files = kraken_file_list_from_uris (uris);
	kraken_directory_notify_files_added (files);
	g_list_free_full (files, g_object_unref);
}

void
kraken_directory_notify_files_changed (GList *files)
{
	GHashTable *changed_lists;
	GList *node;
	GFile *location;
    GFile *parent;
    KrakenDirectory *dir;
	KrakenFile *file;

	/* Make a list of changed files in each directory. */
	changed_lists = g_hash_table_new (NULL, NULL);

	/* Go through all the notifications. */
	for (node = files; node != NULL; node = node->next) {
		location = node->data;

		/* Find the file. */
		file = kraken_file_get_existing (location);
		if (file != NULL) {
			/* Tell it to re-get info now, and later emit
			 * a changed signal.
			 */
			file->details->file_info_is_up_to_date = FALSE;
			file->details->link_info_is_up_to_date = FALSE;
			kraken_file_invalidate_extension_info_internal (file);

			hash_table_list_prepend (changed_lists,
						 file->details->directory,
						 file);
        } else {
            parent = g_file_get_parent (location);
            dir = kraken_directory_get_existing (parent);
            if (dir != NULL && dir->details->new_files_in_progress != NULL &&
                files != dir->details->new_files_in_progress_changes) {
                dir->details->new_files_in_progress_changes = g_list_prepend (dir->details->new_files_in_progress_changes,
                                                                              g_object_ref (location));
            }

            if (dir != NULL) {
                kraken_directory_unref (dir);
            }

            if (parent != NULL) {
                g_object_unref (parent);
            }
        }
    }

	/* Now send out the changed signals. */
	g_hash_table_foreach (changed_lists, call_files_changed_unref_free_list, NULL);
	g_hash_table_destroy (changed_lists);
}

void
kraken_directory_notify_files_changed_by_uri (GList *uris)
{
	GList *files;

	files = kraken_file_list_from_uris (uris);
	kraken_directory_notify_files_changed (files);
	g_list_free_full (files, g_object_unref);
}

void
kraken_directory_notify_files_removed (GList *files)
{
	GHashTable *changed_lists;
	GList *p;
	KrakenDirectory *directory;
	GHashTable *parent_directories;
	KrakenFile *file;
	GFile *location;

	/* Make a list of changed files in each directory. */
	changed_lists = g_hash_table_new (NULL, NULL);

	/* Make a list of parent directories that will need their counts updated. */
	parent_directories = g_hash_table_new (NULL, NULL);

	/* Go through all the notifications. */
	for (p = files; p != NULL; p = p->next) {
		location = p->data;

		/* Update file count for parent directory if anyone might care. */
		directory = get_parent_directory_if_exists (location);
		if (directory != NULL) {
			collect_parent_directories (parent_directories, directory);
			kraken_directory_unref (directory);
		}

		/* Find the file. */
		file = kraken_file_get_existing (location);
		if (file != NULL && !kraken_file_rename_in_progress (file)) {
			/* Mark it gone and prepare to send the changed signal. */
			kraken_file_mark_gone (file);
			hash_table_list_prepend (changed_lists,
						 file->details->directory,
						 kraken_file_ref (file));
		}
		kraken_file_unref (file);
	}

	/* Now send out the changed signals. */
	g_hash_table_foreach (changed_lists, call_files_changed_unref_free_list, NULL);
	g_hash_table_destroy (changed_lists);

	/* Invalidate count for each parent directory. */
	g_hash_table_foreach (parent_directories, invalidate_count_and_unref, NULL);
	g_hash_table_destroy (parent_directories);
}

void
kraken_directory_notify_files_removed_by_uri (GList *uris)
{
	GList *files;

	files = kraken_file_list_from_uris (uris);
	kraken_directory_notify_files_changed (files);
	g_list_free_full (files, g_object_unref);
}

static void
set_directory_location (KrakenDirectory *directory,
			GFile *location)
{
	if (directory->details->location) {
		g_object_unref (directory->details->location);
	}
	directory->details->location = g_object_ref (location);
	
}

static void
change_directory_location (KrakenDirectory *directory,
			   GFile *new_location)
{
	/* I believe it's impossible for a self-owned file/directory
	 * to be moved. But if that did somehow happen, this function
	 * wouldn't do enough to handle it.
	 */
	g_assert (directory->details->as_file == NULL);

	g_hash_table_remove (directories,
			     directory->details->location);

	set_directory_location (directory, new_location);

	g_hash_table_insert (directories,
			     directory->details->location,
			     directory);
}

typedef struct {
	GFile *container;
	GList *directories;
} CollectData;

static void
collect_directories_by_container (gpointer key, gpointer value, gpointer callback_data)
{
	KrakenDirectory *directory;
	CollectData *collect_data;
	GFile *location;

	location = (GFile *) key;
	directory = KRAKEN_DIRECTORY (value);
	collect_data = (CollectData *) callback_data;

	if (g_file_has_prefix (location, collect_data->container) ||
	    g_file_equal (collect_data->container, location)) {
		kraken_directory_ref (directory);
		collect_data->directories =
			g_list_prepend (collect_data->directories,
					directory);
	}
}

static GList *
kraken_directory_moved_internal (GFile *old_location,
				   GFile *new_location)
{
	CollectData collection;
	KrakenDirectory *directory;
	GList *node, *affected_files;
	GFile *new_directory_location;
	char *relative_path;

	collection.container = old_location;
	collection.directories = NULL;

	g_hash_table_foreach (directories,
			      collect_directories_by_container,
			      &collection);

	affected_files = NULL;

	for (node = collection.directories; node != NULL; node = node->next) {
		directory = KRAKEN_DIRECTORY (node->data);
		new_directory_location = NULL;

		if (g_file_equal (directory->details->location, old_location)) {
			new_directory_location = g_object_ref (new_location);
		} else {
			relative_path = g_file_get_relative_path (old_location,
								  directory->details->location);
			if (relative_path != NULL) {
				new_directory_location = g_file_resolve_relative_path (new_location, relative_path);
				g_free (relative_path);
				
			}
		}
		
		if (new_directory_location) {
			change_directory_location (directory, new_directory_location);
			g_object_unref (new_directory_location);
		
			/* Collect affected files. */
			if (directory->details->as_file != NULL) {
				affected_files = g_list_prepend
					(affected_files,
					 kraken_file_ref (directory->details->as_file));
			}
			affected_files = g_list_concat
				(affected_files,
				 kraken_file_list_copy (directory->details->file_list));
		}
		
		kraken_directory_unref (directory);
	}

	g_list_free (collection.directories);

	return affected_files;
}

void
kraken_directory_moved (const char *old_uri,
			  const char *new_uri)
{
	GList *list, *node;
	GHashTable *hash;
	KrakenFile *file;
	GFile *old_location;
	GFile *new_location;

	hash = g_hash_table_new (NULL, NULL);

	old_location = g_file_new_for_uri (old_uri);
	new_location = g_file_new_for_uri (new_uri);
	
	list = kraken_directory_moved_internal (old_location, new_location);
	for (node = list; node != NULL; node = node->next) {
		file = KRAKEN_FILE (node->data);
		hash_table_list_prepend (hash,
					 file->details->directory,
					 kraken_file_ref (file));
	}
	kraken_file_list_free (list);
	
	g_object_unref (old_location);
	g_object_unref (new_location);

	g_hash_table_foreach (hash, call_files_changed_unref_free_list, NULL);
	g_hash_table_destroy (hash);
}

void
kraken_directory_notify_files_moved (GList *file_pairs)
{
	GList *p, *affected_files, *node;
	GFilePair *pair;
	KrakenFile *file;
	KrakenDirectory *old_directory, *new_directory;
	GHashTable *parent_directories;
	GList *new_files_list, *unref_list;
	GHashTable *added_lists, *changed_lists;
	char *name;
	KrakenFileAttributes cancel_attributes;
	GFile *to_location, *from_location;
	
	/* Make a list of added and changed files in each directory. */
	new_files_list = NULL;
	added_lists = g_hash_table_new (NULL, NULL);
	changed_lists = g_hash_table_new (NULL, NULL);
	unref_list = NULL;

	/* Make a list of parent directories that will need their counts updated. */
	parent_directories = g_hash_table_new (NULL, NULL);

	cancel_attributes = kraken_file_get_all_attributes ();

	for (p = file_pairs; p != NULL; p = p->next) {
		pair = p->data;
		from_location = pair->from;
		to_location = pair->to;

		/* Handle overwriting a file. */
		file = kraken_file_get_existing (to_location);
		if (file != NULL) {
			/* Mark it gone and prepare to send the changed signal. */
			kraken_file_mark_gone (file);
			new_directory = file->details->directory;
			hash_table_list_prepend (changed_lists,
						 new_directory,
						 file);
			collect_parent_directories (parent_directories,
						    new_directory);
		}

		/* Update any directory objects that are affected. */
		affected_files = kraken_directory_moved_internal (from_location,
								    to_location);
		for (node = affected_files; node != NULL; node = node->next) {
			file = KRAKEN_FILE (node->data);
			hash_table_list_prepend (changed_lists,
						 file->details->directory,
						 file);
		}
		unref_list = g_list_concat (unref_list, affected_files);

		/* Move an existing file. */
		file = kraken_file_get_existing (from_location);
		if (file == NULL) {
			/* Handle this as if it was a new file. */
			new_files_list = g_list_prepend (new_files_list,
							 to_location);
		} else {
			/* Handle notification in the old directory. */
			old_directory = file->details->directory;
			collect_parent_directories (parent_directories, old_directory);

			/* Cancel loading of attributes in the old directory */
			kraken_directory_cancel_loading_file_attributes
				(old_directory, file, cancel_attributes);

			/* Locate the new directory. */
			new_directory = get_parent_directory (to_location);
			collect_parent_directories (parent_directories, new_directory);
			/* We can unref now -- new_directory is in the
			 * parent directories list so it will be
			 * around until the end of this function
			 * anyway.
			 */
			kraken_directory_unref (new_directory);

			/* Update the file's name and directory. */
			name = g_file_get_basename (to_location);
			kraken_file_update_name_and_directory 
				(file, name, new_directory);
			g_free (name);

			/* Update file attributes */
			kraken_file_invalidate_attributes (file, KRAKEN_FILE_ATTRIBUTE_INFO);

			hash_table_list_prepend (changed_lists,
						 old_directory,
						 file);
			if (old_directory != new_directory) {
				hash_table_list_prepend	(added_lists,
							 new_directory,
							 file);
			}

			/* Unref each file once to balance out kraken_file_get_by_uri. */
			unref_list = g_list_prepend (unref_list, file);
		}
	}

	/* Now send out the changed and added signals for existing file objects. */
	g_hash_table_foreach (changed_lists, call_files_changed_free_list, NULL);
	g_hash_table_destroy (changed_lists);
	g_hash_table_foreach (added_lists, call_files_added_free_list, NULL);
	g_hash_table_destroy (added_lists);

	/* Let the file objects go. */
	kraken_file_list_free (unref_list);

	/* Invalidate count for each parent directory. */
	g_hash_table_foreach (parent_directories, invalidate_count_and_unref, NULL);
	g_hash_table_destroy (parent_directories);

	/* Separate handling for brand new file objects. */
	kraken_directory_notify_files_added (new_files_list);
	g_list_free (new_files_list);
}

void
kraken_directory_notify_files_moved_by_uri (GList *uri_pairs)
{
	GList *file_pairs;

	file_pairs = uri_pairs_to_file_pairs (uri_pairs);
	kraken_directory_notify_files_moved (file_pairs);
	g_list_foreach (file_pairs, (GFunc)g_file_pair_free, NULL);
	g_list_free (file_pairs);
}

void
kraken_directory_schedule_position_set (GList *position_setting_list)
{
	GList *p;
	const KrakenFileChangesQueuePosition *item;
	KrakenFile *file;
	time_t now;

	time (&now);

	for (p = position_setting_list; p != NULL; p = p->next) {
		item = (KrakenFileChangesQueuePosition *) p->data;

		file = kraken_file_get (item->location);

		if (item->set) {
            kraken_file_set_position (file, item->point.x, item->point.y);
		} else {
            kraken_file_set_position (file, -1, -1);
		}

        if (item->set) {
            kraken_file_set_monitor_number (file, item->monitor);
        } else {
            kraken_file_set_monitor_number (file, -1);
        }

        kraken_file_set_time_metadata (file, KRAKEN_METADATA_KEY_ICON_POSITION_TIMESTAMP, UNDEFINED_TIME);

		kraken_file_unref (file);
	}
}

gboolean
kraken_directory_contains_file (KrakenDirectory *directory,
				  KrakenFile *file)
{
	g_return_val_if_fail (KRAKEN_IS_DIRECTORY (directory), FALSE);
	g_return_val_if_fail (KRAKEN_IS_FILE (file), FALSE);

	if (kraken_file_is_gone (file)) {
		return FALSE;
	}

	return KRAKEN_DIRECTORY_CLASS (G_OBJECT_GET_CLASS (directory))->contains_file (directory, file);
}

char *
kraken_directory_get_file_uri (KrakenDirectory *directory,
				 const char *file_name)
{
	GFile *child;
	char *result;

	g_return_val_if_fail (KRAKEN_IS_DIRECTORY (directory), NULL);
	g_return_val_if_fail (file_name != NULL, NULL);

	result = NULL;

	child = g_file_get_child (directory->details->location, file_name);
	result = g_file_get_uri (child);
	g_object_unref (child);
	
	return result;
}

void
kraken_directory_call_when_ready (KrakenDirectory *directory,
				    KrakenFileAttributes file_attributes,
				    gboolean wait_for_all_files,
				    KrakenDirectoryCallback callback,
				    gpointer callback_data)
{
	g_return_if_fail (KRAKEN_IS_DIRECTORY (directory));
	g_return_if_fail (callback != NULL);

	KRAKEN_DIRECTORY_CLASS (G_OBJECT_GET_CLASS (directory))->call_when_ready 
		(directory, file_attributes, wait_for_all_files,
		 callback, callback_data);
}

void
kraken_directory_cancel_callback (KrakenDirectory *directory,
				    KrakenDirectoryCallback callback,
				    gpointer callback_data)
{
	g_return_if_fail (KRAKEN_IS_DIRECTORY (directory));
	g_return_if_fail (callback != NULL);

	KRAKEN_DIRECTORY_CLASS (G_OBJECT_GET_CLASS (directory))->cancel_callback 
		(directory, callback, callback_data);
}

void
kraken_directory_file_monitor_add (KrakenDirectory *directory,
				     gconstpointer client,
				     gboolean monitor_hidden_files,
				     KrakenFileAttributes file_attributes,
				     KrakenDirectoryCallback callback,
				     gpointer callback_data)
{
	g_return_if_fail (KRAKEN_IS_DIRECTORY (directory));
	g_return_if_fail (client != NULL);

	KRAKEN_DIRECTORY_CLASS (G_OBJECT_GET_CLASS (directory))->file_monitor_add 
		(directory, client,
		 monitor_hidden_files,
		 file_attributes,
		 callback, callback_data);
}

void
kraken_directory_file_monitor_remove (KrakenDirectory *directory,
					gconstpointer client)
{
	g_return_if_fail (KRAKEN_IS_DIRECTORY (directory));
	g_return_if_fail (client != NULL);

	KRAKEN_DIRECTORY_CLASS (G_OBJECT_GET_CLASS (directory))->file_monitor_remove
		(directory, client);
}

void
kraken_directory_force_reload (KrakenDirectory *directory)
{
	g_return_if_fail (KRAKEN_IS_DIRECTORY (directory));

	KRAKEN_DIRECTORY_CLASS (G_OBJECT_GET_CLASS (directory))->force_reload (directory);
}

gboolean
kraken_directory_is_not_empty (KrakenDirectory *directory)
{
	g_return_val_if_fail (KRAKEN_IS_DIRECTORY (directory), FALSE);

	return KRAKEN_DIRECTORY_CLASS (G_OBJECT_GET_CLASS (directory))->is_not_empty (directory);
}

static gboolean
is_tentative (gpointer data, gpointer callback_data)
{
	KrakenFile *file;

	g_assert (callback_data == NULL);

	file = KRAKEN_FILE (data);
	/* Avoid returning files with !is_added, because these
	 * will later be sent with the files_added signal, and a
	 * user doing get_file_list + files_added monitoring will
	 * then see the file twice */
	return !file->details->got_file_info || !file->details->is_added;
}

GList *
kraken_directory_get_file_list (KrakenDirectory *directory)
{
	return KRAKEN_DIRECTORY_CLASS (G_OBJECT_GET_CLASS (directory))->get_file_list (directory);
}

static GList *
real_get_file_list (KrakenDirectory *directory)
{
	GList *tentative_files, *non_tentative_files;

	tentative_files = eel_g_list_partition
		(g_list_copy (directory->details->file_list),
		 is_tentative, NULL, &non_tentative_files);
	g_list_free (tentative_files);

	kraken_file_list_ref (non_tentative_files);
	return non_tentative_files;
}

static gboolean
real_is_editable (KrakenDirectory *directory)
{
	return TRUE;
}

gboolean
kraken_directory_is_editable (KrakenDirectory *directory)
{
	return KRAKEN_DIRECTORY_CLASS (G_OBJECT_GET_CLASS (directory))->is_editable (directory);
}

GList *
kraken_directory_match_pattern (KrakenDirectory *directory, const char *pattern)
{
	GList *files, *l, *ret;
	GPatternSpec *spec;


	ret = NULL;
	spec = g_pattern_spec_new (pattern);
	
	files = kraken_directory_get_file_list (directory);
	for (l = files; l; l = l->next) {
		KrakenFile *file;
		char *name;
	       
	        file = KRAKEN_FILE (l->data);
		name = kraken_file_get_display_name (file);

		if (g_pattern_match_string (spec, name)) {
			ret = g_list_prepend(ret, kraken_file_ref (file));	
		}

		g_free (name);
	}

	g_pattern_spec_free (spec);
	kraken_file_list_free (files);

	return ret;
}

/**
 * kraken_directory_list_ref
 *
 * Ref all the directories in a list.
 * @list: GList of directories.
 **/
GList *
kraken_directory_list_ref (GList *list)
{
	g_list_foreach (list, (GFunc) kraken_directory_ref, NULL);
	return list;
}

/**
 * kraken_directory_list_unref
 *
 * Unref all the directories in a list.
 * @list: GList of directories.
 **/
void
kraken_directory_list_unref (GList *list)
{
	g_list_foreach (list, (GFunc) kraken_directory_unref, NULL);
}

/**
 * kraken_directory_list_free
 *
 * Free a list of directories after unrefing them.
 * @list: GList of directories.
 **/
void
kraken_directory_list_free (GList *list)
{
	kraken_directory_list_unref (list);
	g_list_free (list);
}

/**
 * kraken_directory_list_copy
 *
 * Copy the list of directories, making a new ref of each,
 * @list: GList of directories.
 **/
GList *
kraken_directory_list_copy (GList *list)
{
	return g_list_copy (kraken_directory_list_ref (list));
}

static int
compare_by_uri (KrakenDirectory *a, KrakenDirectory *b)
{
	char *uri_a, *uri_b;
	int res;

	uri_a = g_file_get_uri (a->details->location);
	uri_b = g_file_get_uri (b->details->location);
	
	res = strcmp (uri_a, uri_b);

	g_free (uri_a);
	g_free (uri_b);
	
	return res;
}

static int
compare_by_uri_cover (gconstpointer a, gconstpointer b)
{
	return compare_by_uri (KRAKEN_DIRECTORY (a), KRAKEN_DIRECTORY (b));
}

/**
 * kraken_directory_list_sort_by_uri
 * 
 * Sort the list of directories by directory uri.
 * @list: GList of directories.
 **/
GList *
kraken_directory_list_sort_by_uri (GList *list)
{
	return g_list_sort (list, compare_by_uri_cover);
}

gboolean
kraken_directory_is_desktop_directory (KrakenDirectory   *directory)
{
	if (directory->details->location == NULL) {
		return FALSE;
	}

	return kraken_is_desktop_directory (directory->details->location);
}

void
kraken_directory_set_show_thumbnails (KrakenDirectory         *directory,
                                    gboolean show_thumbnails)
{
  KrakenFile *file;
  
  file = kraken_file_get(directory->details->location);
  kraken_file_set_boolean_metadata (file, KRAKEN_METADATA_KEY_SHOW_THUMBNAILS, FALSE, show_thumbnails);
  kraken_directory_force_reload (directory);
  kraken_file_unref (file);
}

#if !defined (KRAKEN_OMIT_SELF_CHECK)

#include <eel/eel-debug.h>
#include "kraken-file-attributes.h"

static int data_dummy;
static gboolean got_files_flag;

static void
got_files_callback (KrakenDirectory *directory, GList *files, gpointer callback_data)
{
	g_assert (KRAKEN_IS_DIRECTORY (directory));
	g_assert (g_list_length (files) > 10);
	g_assert (callback_data == &data_dummy);

	got_files_flag = TRUE;
}

/* Return the number of extant KrakenDirectories */
int
kraken_directory_number_outstanding (void)
{
        return directories ? g_hash_table_size (directories) : 0;
}

void
kraken_self_check_directory (void)
{
	KrakenDirectory *directory;
	KrakenFile *file;

	directory = kraken_directory_get_by_uri ("file:///etc");
	file = kraken_file_get_by_uri ("file:///etc/passwd");

	EEL_CHECK_INTEGER_RESULT (g_hash_table_size (directories), 1);

	kraken_directory_file_monitor_add
		(directory, &data_dummy,
		 TRUE, 0, NULL, NULL);

	/* FIXME: these need to be updated to the new metadata infrastructure
	 *  as make check doesn't pass.
	kraken_file_set_metadata (file, "test", "default", "value");
	EEL_CHECK_STRING_RESULT (kraken_file_get_metadata (file, "test", "default"), "value");

	kraken_file_set_boolean_metadata (file, "test_boolean", TRUE, TRUE);
	EEL_CHECK_BOOLEAN_RESULT (kraken_file_get_boolean_metadata (file, "test_boolean", TRUE), TRUE);
	kraken_file_set_boolean_metadata (file, "test_boolean", TRUE, FALSE);
	EEL_CHECK_BOOLEAN_RESULT (kraken_file_get_boolean_metadata (file, "test_boolean", TRUE), FALSE);
	EEL_CHECK_BOOLEAN_RESULT (kraken_file_get_boolean_metadata (NULL, "test_boolean", TRUE), TRUE);

	kraken_file_set_integer_metadata (file, "test_integer", 0, 17);
	EEL_CHECK_INTEGER_RESULT (kraken_file_get_integer_metadata (file, "test_integer", 0), 17);
	kraken_file_set_integer_metadata (file, "test_integer", 0, -1);
	EEL_CHECK_INTEGER_RESULT (kraken_file_get_integer_metadata (file, "test_integer", 0), -1);
	kraken_file_set_integer_metadata (file, "test_integer", 42, 42);
	EEL_CHECK_INTEGER_RESULT (kraken_file_get_integer_metadata (file, "test_integer", 42), 42);
	EEL_CHECK_INTEGER_RESULT (kraken_file_get_integer_metadata (NULL, "test_integer", 42), 42);
	EEL_CHECK_INTEGER_RESULT (kraken_file_get_integer_metadata (file, "nonexistent_key", 42), 42);
	*/

	EEL_CHECK_BOOLEAN_RESULT (kraken_directory_get_by_uri ("file:///etc") == directory, TRUE);
	kraken_directory_unref (directory);

	EEL_CHECK_BOOLEAN_RESULT (kraken_directory_get_by_uri ("file:///etc/") == directory, TRUE);
	kraken_directory_unref (directory);

	EEL_CHECK_BOOLEAN_RESULT (kraken_directory_get_by_uri ("file:///etc////") == directory, TRUE);
	kraken_directory_unref (directory);

	kraken_file_unref (file);

	kraken_directory_file_monitor_remove (directory, &data_dummy);

	kraken_directory_unref (directory);

	while (g_hash_table_size (directories) != 0) {
		gtk_main_iteration ();
	}

	EEL_CHECK_INTEGER_RESULT (g_hash_table_size (directories), 0);

	directory = kraken_directory_get_by_uri ("file:///etc");

	got_files_flag = FALSE;

	kraken_directory_call_when_ready (directory,
					    KRAKEN_FILE_ATTRIBUTE_INFO |
					    KRAKEN_FILE_ATTRIBUTE_DEEP_COUNTS,
					    TRUE,
					    got_files_callback, &data_dummy);

	while (!got_files_flag) {
		gtk_main_iteration ();
	}

	EEL_CHECK_BOOLEAN_RESULT (directory->details->file_list == NULL, TRUE);

	EEL_CHECK_INTEGER_RESULT (g_hash_table_size (directories), 1);

	file = kraken_file_get_by_uri ("file:///etc/passwd");

	/* EEL_CHECK_STRING_RESULT (kraken_file_get_metadata (file, "test", "default"), "value"); */
	
	kraken_file_unref (file);

	kraken_directory_unref (directory);

	EEL_CHECK_INTEGER_RESULT (g_hash_table_size (directories), 0);
}

#endif /* !KRAKEN_OMIT_SELF_CHECK */
