/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   kraken-directory.h: Kraken directory model.
 
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

#ifndef KRAKEN_DIRECTORY_H
#define KRAKEN_DIRECTORY_H

#include <gtk/gtk.h>
#include <gio/gio.h>
#include <libkraken-private/kraken-file-attributes.h>

/* KrakenDirectory is a class that manages the model for a directory,
   real or virtual, for Kraken, mainly the file-manager component. The directory is
   responsible for managing both real data and cached metadata. On top of
   the file system independence provided by gio, the directory
   object also provides:
  
       1) A synchronization framework, which notifies via signals as the
          set of known files changes.
       2) An abstract interface for getting attributes and performing
          operations on files.
*/

#define KRAKEN_TYPE_DIRECTORY kraken_directory_get_type()
#define KRAKEN_DIRECTORY(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_DIRECTORY, KrakenDirectory))
#define KRAKEN_DIRECTORY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_DIRECTORY, KrakenDirectoryClass))
#define KRAKEN_IS_DIRECTORY(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_DIRECTORY))
#define KRAKEN_IS_DIRECTORY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_DIRECTORY))
#define KRAKEN_DIRECTORY_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_DIRECTORY, KrakenDirectoryClass))

/* KrakenFile is defined both here and in kraken-file.h. */
#ifndef KRAKEN_FILE_DEFINED
#define KRAKEN_FILE_DEFINED
typedef struct KrakenFile KrakenFile;
#endif

typedef struct KrakenDirectoryDetails KrakenDirectoryDetails;

typedef struct _KrakenDirectory KrakenDirectory;
struct _KrakenDirectory
{
	GObject object;
	KrakenDirectoryDetails *details;
};

typedef void (*KrakenDirectoryCallback) (KrakenDirectory *directory,
					   GList             *files,
					   gpointer           callback_data);

typedef struct
{
	GObjectClass parent_class;

	/*** Notification signals for clients to connect to. ***/

	/* The files_added signal is emitted as the directory model 
	 * discovers new files.
	 */
	void     (* files_added)         (KrakenDirectory          *directory,
					  GList                      *added_files);

	/* The files_changed signal is emitted as changes occur to
	 * existing files that are noticed by the synchronization framework,
	 * including when an old file has been deleted. When an old file
	 * has been deleted, this is the last chance to forget about these
	 * file objects, which are about to be unref'd. Use a call to
	 * kraken_file_is_gone () to test for this case.
	 */
	void     (* files_changed)       (KrakenDirectory         *directory,
					  GList                     *changed_files);

	/* The done_loading signal is emitted when a directory load
	 * request completes. This is needed because, at least in the
	 * case where the directory is empty, the caller will receive
	 * no kind of notification at all when a directory load
	 * initiated by `kraken_directory_file_monitor_add' completes.
	 */
	void     (* done_loading)        (KrakenDirectory         *directory);

	void     (* load_error)          (KrakenDirectory         *directory,
					  GError                    *error);

	/*** Virtual functions for subclasses to override. ***/
	gboolean (* contains_file)       (KrakenDirectory         *directory,
					  KrakenFile              *file);
	void     (* call_when_ready)     (KrakenDirectory         *directory,
					  KrakenFileAttributes     file_attributes,
					  gboolean                   wait_for_file_list,
					  KrakenDirectoryCallback  callback,
					  gpointer                   callback_data);
	void     (* cancel_callback)     (KrakenDirectory         *directory,
					  KrakenDirectoryCallback  callback,
					  gpointer                   callback_data);
	void     (* file_monitor_add)    (KrakenDirectory          *directory,
					  gconstpointer              client,
					  gboolean                   monitor_hidden_files,
					  KrakenFileAttributes     monitor_attributes,
					  KrakenDirectoryCallback  initial_files_callback,
					  gpointer                   callback_data);
	void     (* file_monitor_remove) (KrakenDirectory         *directory,
					  gconstpointer              client);
	void     (* force_reload)        (KrakenDirectory         *directory);
	gboolean (* are_all_files_seen)  (KrakenDirectory         *directory);
	gboolean (* is_not_empty)        (KrakenDirectory         *directory);

	/* get_file_list is a function pointer that subclasses may override to
	 * customize collecting the list of files in a directory.
	 * For example, the KrakenDesktopDirectory overrides this so that it can
	 * merge together the list of files in the $HOME/Desktop directory with
	 * the list of standard icons (Computer, Home, Trash) on the desktop.
	 */
	GList *	 (* get_file_list)	 (KrakenDirectory *directory);

	/* Should return FALSE if the directory is read-only and doesn't
	 * allow setting of metadata.
	 * An example of this is the search directory.
	 */
	gboolean (* is_editable)         (KrakenDirectory *directory);
} KrakenDirectoryClass;

/* Basic GObject requirements. */
GType              kraken_directory_get_type                 (void);

/* Get a directory given a uri.
 * Creates the appropriate subclass given the uri mappings.
 * Returns a referenced object, not a floating one. Unref when finished.
 * If two windows are viewing the same uri, the directory object is shared.
 */
KrakenDirectory *kraken_directory_get                      (GFile                     *location);
KrakenDirectory *kraken_directory_get_by_uri               (const char                *uri);
KrakenDirectory *kraken_directory_get_for_file             (KrakenFile              *file);

/* Covers for g_object_ref and g_object_unref that provide two conveniences:
 * 1) Using these is type safe.
 * 2) You are allowed to call these with NULL,
 */
KrakenDirectory *kraken_directory_ref                      (KrakenDirectory         *directory);
void               kraken_directory_unref                    (KrakenDirectory         *directory);

/* Access to a URI. */
char *             kraken_directory_get_uri                  (KrakenDirectory         *directory);
GFile *            kraken_directory_get_location             (KrakenDirectory         *directory);

/* Is this file still alive and in this directory? */
gboolean           kraken_directory_contains_file            (KrakenDirectory         *directory,
								KrakenFile              *file);

/* Get the uri of the file in the directory, NULL if not found */
char *             kraken_directory_get_file_uri             (KrakenDirectory         *directory,
								const char                *file_name);

/* Get (and ref) a KrakenFile object for this directory. */
KrakenFile *     kraken_directory_get_corresponding_file   (KrakenDirectory         *directory);

/* Waiting for data that's read asynchronously.
 * The file attribute and metadata keys are for files in the directory.
 */
void               kraken_directory_call_when_ready          (KrakenDirectory         *directory,
								KrakenFileAttributes     file_attributes,
								gboolean                   wait_for_all_files,
								KrakenDirectoryCallback  callback,
								gpointer                   callback_data);
void               kraken_directory_cancel_callback          (KrakenDirectory         *directory,
								KrakenDirectoryCallback  callback,
								gpointer                   callback_data);


/* Monitor the files in a directory. */
void               kraken_directory_file_monitor_add         (KrakenDirectory         *directory,
								gconstpointer              client,
								gboolean                   monitor_hidden_files,
								KrakenFileAttributes     attributes,
								KrakenDirectoryCallback  initial_files_callback,
								gpointer                   callback_data);
void               kraken_directory_file_monitor_remove      (KrakenDirectory         *directory,
								gconstpointer              client);
void               kraken_directory_force_reload             (KrakenDirectory         *directory);

/* Get a list of all files currently known in the directory. */
GList *            kraken_directory_get_file_list            (KrakenDirectory         *directory);

GList *            kraken_directory_match_pattern            (KrakenDirectory         *directory,
							        const char *glob);


/* Return true if the directory has information about all the files.
 * This will be false until the directory has been read at least once.
 */
gboolean           kraken_directory_are_all_files_seen       (KrakenDirectory         *directory);

/* Return true if the directory is local. */
gboolean           kraken_directory_is_local                 (KrakenDirectory         *directory);

gboolean           kraken_directory_is_in_trash              (KrakenDirectory         *directory);
gboolean           kraken_directory_is_in_recent             (KrakenDirectory         *directory);
gboolean           kraken_directory_is_in_favorites          (KrakenDirectory         *directory);
gboolean           kraken_directory_is_in_admin              (KrakenDirectory         *directory);
gboolean           kraken_directory_is_in_search             (KrakenDirectory         *directory);
gboolean           kraken_directory_is_in_tags               (KrakenDirectory         *directory);
/* Return false if directory contains anything besides a Kraken metafile.
 * Only valid if directory is monitored. Used by the Trash monitor.
 */
gboolean           kraken_directory_is_not_empty             (KrakenDirectory         *directory);

/* Convenience functions for dealing with a list of KrakenDirectory objects that each have a ref.
 * These are just convenient names for functions that work on lists of GtkObject *.
 */
GList *            kraken_directory_list_ref                 (GList                     *directory_list);
void               kraken_directory_list_unref               (GList                     *directory_list);
void               kraken_directory_list_free                (GList                     *directory_list);
GList *            kraken_directory_list_copy                (GList                     *directory_list);
GList *            kraken_directory_list_sort_by_uri         (GList                     *directory_list);

/* Fast way to check if a directory is the desktop directory */
gboolean           kraken_directory_is_desktop_directory     (KrakenDirectory         *directory);

gboolean           kraken_directory_is_editable              (KrakenDirectory         *directory);

void               kraken_directory_set_show_thumbnails      (KrakenDirectory         *directory,
                                gboolean show_thumbnails);

#endif /* KRAKEN_DIRECTORY_H */
