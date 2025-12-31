/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   kraken-file.h: Kraken file model.
 
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

#ifndef KRAKEN_FILE_H
#define KRAKEN_FILE_H

#include <gtk/gtk.h>
#include <gio/gio.h>
#include <pango/pango.h>
#include <libkraken-private/kraken-file-attributes.h>
#include <libkraken-private/kraken-icon-info.h>
#include <libkraken-private/kraken-search-engine.h>

/* KrakenFile is an object used to represent a single element of a
 * KrakenDirectory. It's lightweight and relies on KrakenDirectory
 * to do most of the work.
 */

/* KrakenFile is defined both here and in kraken-directory.h. */
#ifndef KRAKEN_FILE_DEFINED
#define KRAKEN_FILE_DEFINED
typedef struct KrakenFile KrakenFile;
#endif

#define KRAKEN_TYPE_FILE kraken_file_get_type()
#define KRAKEN_FILE(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_FILE, KrakenFile))
#define KRAKEN_FILE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_FILE, KrakenFileClass))
#define KRAKEN_IS_FILE(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_FILE))
#define KRAKEN_IS_FILE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_FILE))
#define KRAKEN_FILE_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_FILE, KrakenFileClass))

typedef enum {
	KRAKEN_FILE_SORT_NONE,
	KRAKEN_FILE_SORT_BY_DISPLAY_NAME,
	KRAKEN_FILE_SORT_BY_SIZE,
	KRAKEN_FILE_SORT_BY_TYPE,
    KRAKEN_FILE_SORT_BY_DETAILED_TYPE,
	KRAKEN_FILE_SORT_BY_MTIME,
    KRAKEN_FILE_SORT_BY_ATIME,
	KRAKEN_FILE_SORT_BY_TRASHED_TIME,
    KRAKEN_FILE_SORT_BY_BTIME,
    KRAKEN_FILE_SORT_BY_SEARCH_RESULT_COUNT
} KrakenFileSortType;	

typedef enum {
	KRAKEN_REQUEST_NOT_STARTED,
	KRAKEN_REQUEST_IN_PROGRESS,
	KRAKEN_REQUEST_DONE
} KrakenRequestStatus;

typedef enum {
	KRAKEN_FILE_ICON_FLAGS_NONE = 0,
	KRAKEN_FILE_ICON_FLAGS_USE_THUMBNAILS = (1<<0),
	KRAKEN_FILE_ICON_FLAGS_IGNORE_VISITING = (1<<1),
	KRAKEN_FILE_ICON_FLAGS_EMBEDDING_TEXT = (1<<2),
	KRAKEN_FILE_ICON_FLAGS_FOR_DRAG_ACCEPT = (1<<3),
	KRAKEN_FILE_ICON_FLAGS_FOR_OPEN_FOLDER = (1<<4),
	/* whether the thumbnail size must match the display icon size */
	KRAKEN_FILE_ICON_FLAGS_FORCE_THUMBNAIL_SIZE = (1<<5),
	/* uses the icon of the mount if present */
	KRAKEN_FILE_ICON_FLAGS_USE_MOUNT_ICON = (1<<6),
	/* render the mount icon as an emblem over the regular one */
	KRAKEN_FILE_ICON_FLAGS_USE_MOUNT_ICON_AS_EMBLEM = (1<<7),
    KRAKEN_FILE_ICON_FLAGS_PIN_HEIGHT_FOR_DESKTOP = (1<<8)
} KrakenFileIconFlags;	

typedef enum {
    KRAKEN_DATE_TYPE_MODIFIED,
    KRAKEN_DATE_TYPE_CHANGED,
    KRAKEN_DATE_TYPE_ACCESSED,
    KRAKEN_DATE_TYPE_PERMISSIONS_CHANGED,
    KRAKEN_DATE_TYPE_TRASHED,
    KRAKEN_DATE_TYPE_CREATED
} KrakenDateType;

typedef enum {
    KRAKEN_FILE_TOOLTIP_FLAGS_NONE = 0,
    KRAKEN_FILE_TOOLTIP_FLAGS_FILE_TYPE =  (1<<0),
    KRAKEN_FILE_TOOLTIP_FLAGS_MOD_DATE = (1<<1),
    KRAKEN_FILE_TOOLTIP_FLAGS_ACCESS_DATE = (1<<2),
    KRAKEN_FILE_TOOLTIP_FLAGS_PATH = (1<<3),
    KRAKEN_FILE_TOOLTIP_FLAGS_CREATED_DATE = (1<<4)
} KrakenFileTooltipFlags;

typedef enum {
    KRAKEN_FILE_LOAD_DEFERRED_ATTRS_NO,
    KRAKEN_FILE_LOAD_DEFERRED_ATTRS_YES,
    KRAKEN_FILE_LOAD_DEFERRED_ATTRS_PRELOAD
} KrakenFileLoadDeferredAttrs;

/* Emblems sometimes displayed for KrakenFiles. Do not localize. */ 
#define KRAKEN_FILE_EMBLEM_NAME_SYMBOLIC_LINK "symbolic-link"
#define KRAKEN_FILE_EMBLEM_NAME_CANT_READ "unreadable"
#define KRAKEN_FILE_EMBLEM_NAME_CANT_WRITE "readonly"
#define KRAKEN_FILE_EMBLEM_NAME_TRASH "trash"
#define KRAKEN_FILE_EMBLEM_NAME_NOTE "note"
#define KRAKEN_FILE_EMBLEM_NAME_FAVORITE "xapp-favorite"

typedef void (*KrakenFileCallback)          (KrakenFile  *file,
				               gpointer       callback_data);
typedef void (*KrakenFileListCallback)      (GList         *file_list,
				               gpointer       callback_data);
typedef void (*KrakenFileOperationCallback) (KrakenFile  *file,
					       GFile         *result_location,
					       GError        *error,
					       gpointer       callback_data);
typedef int (*KrakenWidthMeasureCallback)   (const char    *string,
					       void	     *context);
typedef char * (*KrakenTruncateCallback)    (const char    *string,
					       int	      width,
					       void	     *context);

#define KRAKEN_FILE_ATTRIBUTES_FOR_ICON (KRAKEN_FILE_ATTRIBUTE_INFO | KRAKEN_FILE_ATTRIBUTE_LINK_INFO | KRAKEN_FILE_ATTRIBUTE_THUMBNAIL)
#define KRAKEN_FILE_DEFERRED_ATTRIBUTES (KRAKEN_FILE_ATTRIBUTE_THUMBNAIL | KRAKEN_FILE_ATTRIBUTE_EXTENSION_INFO)

typedef void KrakenFileListHandle;

/* GObject requirements. */
GType                   kraken_file_get_type                          (void);

/* Getting at a single file. */
KrakenFile *          kraken_file_get                               (GFile                          *location);
KrakenFile *          kraken_file_get_by_uri                        (const char                     *uri);

/* Get a file only if the kraken version already exists */
KrakenFile *          kraken_file_get_existing                      (GFile                          *location);
KrakenFile *          kraken_file_get_existing_by_uri               (const char                     *uri);

/* Covers for g_object_ref and g_object_unref that provide two conveniences:
 * 1) Using these is type safe.
 * 2) You are allowed to call these with NULL,
 */
KrakenFile *          kraken_file_ref                               (KrakenFile                   *file);
void                    kraken_file_unref                             (KrakenFile                   *file);

/* Monitor the file. */
void                    kraken_file_monitor_add                       (KrakenFile                   *file,
									 gconstpointer                   client,
									 KrakenFileAttributes          attributes);
void                    kraken_file_monitor_remove                    (KrakenFile                   *file,
									 gconstpointer                   client);

/* Waiting for data that's read asynchronously.
 * This interface currently works only for metadata, but could be expanded
 * to other attributes as well.
 */
void                    kraken_file_call_when_ready                   (KrakenFile                   *file,
									 KrakenFileAttributes          attributes,
									 KrakenFileCallback            callback,
									 gpointer                        callback_data);
void                    kraken_file_cancel_call_when_ready            (KrakenFile                   *file,
									 KrakenFileCallback            callback,
									 gpointer                        callback_data);
gboolean                kraken_file_check_if_ready                    (KrakenFile                   *file,
									 KrakenFileAttributes          attributes);
void                    kraken_file_invalidate_attributes             (KrakenFile                   *file,
									 KrakenFileAttributes          attributes);
void                    kraken_file_invalidate_all_attributes         (KrakenFile                   *file);

void                    kraken_file_increment_thumbnail_try_count     (KrakenFile                   *file);

/* Basic attributes for file objects. */
gboolean                kraken_file_contains_text                     (KrakenFile                   *file);
char *                  kraken_file_get_display_name                  (KrakenFile                   *file);
char *                  kraken_file_get_edit_name                     (KrakenFile                   *file);
char *                  kraken_file_get_name                          (KrakenFile                   *file);
const char *            kraken_file_peek_name                         (KrakenFile                   *file);

GFile *                 kraken_file_get_location                      (KrakenFile                   *file);
char *			 kraken_file_get_description			 (KrakenFile			 *file);
char *                  kraken_file_get_uri                           (KrakenFile                   *file);
char *                  kraken_file_get_local_uri                     (KrakenFile                   *file);
char *                  kraken_file_get_path                          (KrakenFile                   *file);
char *                  kraken_file_get_uri_scheme                    (KrakenFile                   *file);
gboolean                kraken_file_has_uri_scheme                    (KrakenFile *file, const gchar *scheme);
KrakenFile *          kraken_file_get_parent                        (KrakenFile                   *file);
GFile *                 kraken_file_get_parent_location               (KrakenFile                   *file);
char *                  kraken_file_get_parent_uri                    (KrakenFile                   *file);
char *                  kraken_file_get_parent_uri_for_display        (KrakenFile                   *file);
gboolean                kraken_file_can_get_size                      (KrakenFile                   *file);
goffset                 kraken_file_get_size                          (KrakenFile                   *file);
time_t                  kraken_file_get_mtime                         (KrakenFile                   *file);
time_t                  kraken_file_get_ctime                         (KrakenFile                   *file);
GFileType               kraken_file_get_file_type                     (KrakenFile                   *file);
char *                  kraken_file_get_mime_type                     (KrakenFile                   *file);
gboolean                kraken_file_is_mime_type                      (KrakenFile                   *file,
									 const char                     *mime_type);
gboolean                kraken_file_is_launchable                     (KrakenFile                   *file);
gboolean                kraken_file_is_symbolic_link                  (KrakenFile                   *file);
gboolean                kraken_file_is_mountpoint                     (KrakenFile                   *file);
GMount *                kraken_file_get_mount                         (KrakenFile                   *file);
void                    kraken_file_set_mount                         (KrakenFile                   *file,
                                                                     GMount                     *mount);
char *                  kraken_file_get_volume_free_space             (KrakenFile                   *file);
char *                  kraken_file_get_volume_name                   (KrakenFile                   *file);
char *                  kraken_file_get_symbolic_link_target_path     (KrakenFile                   *file);
char *                  kraken_file_get_symbolic_link_target_uri      (KrakenFile                   *file);
gboolean                kraken_file_is_broken_symbolic_link           (KrakenFile                   *file);
gboolean                kraken_file_is_kraken_link                  (KrakenFile                   *file);
gboolean                kraken_file_is_executable                     (KrakenFile                   *file);
gboolean                kraken_file_is_directory                      (KrakenFile                   *file);
gboolean                kraken_file_is_user_special_directory         (KrakenFile                   *file,
									 GUserDirectory                 special_directory);
gboolean		kraken_file_is_archive			(KrakenFile			*file);
gboolean                kraken_file_is_in_trash                       (KrakenFile                   *file);
gboolean                kraken_file_is_in_recent                      (KrakenFile                   *file);
gboolean                kraken_file_is_in_favorites                   (KrakenFile                   *file);
gboolean                kraken_file_is_in_search                      (KrakenFile                   *file);
gboolean                kraken_file_is_unavailable_favorite           (KrakenFile                   *file);
gboolean                kraken_file_is_in_admin                       (KrakenFile                   *file);
gboolean                kraken_file_is_in_desktop                     (KrakenFile                   *file);
gboolean		kraken_file_is_home				(KrakenFile                   *file);
gboolean                kraken_file_is_desktop_directory              (KrakenFile                   *file);
GError *                kraken_file_get_file_info_error               (KrakenFile                   *file);
gboolean                kraken_file_get_directory_item_count          (KrakenFile                   *file,
									 guint                          *count,
									 gboolean                       *count_unreadable);
void                    kraken_file_recompute_deep_counts             (KrakenFile                   *file);
KrakenRequestStatus   kraken_file_get_deep_counts                   (KrakenFile                   *file,
									 guint                          *directory_count,
									 guint                          *file_count,
									 guint                          *unreadable_directory_count,
                                     guint                          *hidden_count,
									 goffset                         *total_size,
									 gboolean                        force);
gboolean                kraken_file_should_show_thumbnail             (KrakenFile                   *file);
void                    kraken_file_delete_thumbnail                  (KrakenFile                   *file);
gboolean                kraken_file_has_loaded_thumbnail              (KrakenFile                   *file);
gboolean                kraken_file_should_show_directory_item_count  (KrakenFile                   *file);
gboolean                kraken_file_should_show_type                  (KrakenFile                   *file);
GList *                 kraken_file_get_keywords                      (KrakenFile                   *file);
GList *                 kraken_file_get_emblem_icons                  (KrakenFile                   *file,
                                                                     KrakenFile                   *view_file);
gboolean                kraken_file_get_directory_item_mime_types     (KrakenFile                   *file,
									 GList                         **mime_list);

void                    kraken_file_set_attributes                    (KrakenFile                   *file, 
									 GFileInfo                      *attributes,
									 KrakenFileOperationCallback   callback,
									 gpointer                        callback_data);
GFilesystemPreviewType  kraken_file_get_filesystem_use_preview        (KrakenFile *file);

char *                  kraken_file_get_filesystem_id                 (KrakenFile                   *file);

KrakenFile *          kraken_file_get_trash_original_file           (KrakenFile                   *file);

/* Permissions. */
gboolean                kraken_file_can_get_permissions               (KrakenFile                   *file);
gboolean                kraken_file_can_set_permissions               (KrakenFile                   *file);
guint                   kraken_file_get_permissions                   (KrakenFile                   *file);
gboolean                kraken_file_can_get_owner                     (KrakenFile                   *file);
gboolean                kraken_file_can_set_owner                     (KrakenFile                   *file);
gboolean                kraken_file_can_get_group                     (KrakenFile                   *file);
gboolean                kraken_file_can_set_group                     (KrakenFile                   *file);
char *                  kraken_file_get_owner_name                    (KrakenFile                   *file);
char *                  kraken_file_get_group_name                    (KrakenFile                   *file);
GList *                 kraken_get_user_names                         (void);
GList *                 kraken_get_all_group_names                    (void);
GList *                 kraken_file_get_settable_group_names          (KrakenFile                   *file);
gboolean                kraken_file_can_get_selinux_context           (KrakenFile                   *file);
char *                  kraken_file_get_selinux_context               (KrakenFile                   *file);

/* "Capabilities". */
gboolean                kraken_file_can_read                          (KrakenFile                   *file);
gboolean                kraken_file_can_write                         (KrakenFile                   *file);
gboolean                kraken_file_can_execute                       (KrakenFile                   *file);
gboolean                kraken_file_can_rename                        (KrakenFile                   *file);
gboolean                kraken_file_can_delete                        (KrakenFile                   *file);
gboolean                kraken_file_can_trash                         (KrakenFile                   *file);

gboolean                kraken_file_can_mount                         (KrakenFile                   *file);
gboolean                kraken_file_can_unmount                       (KrakenFile                   *file);
gboolean                kraken_file_can_eject                         (KrakenFile                   *file);
gboolean                kraken_file_can_start                         (KrakenFile                   *file);
gboolean                kraken_file_can_start_degraded                (KrakenFile                   *file);
gboolean                kraken_file_can_stop                          (KrakenFile                   *file);
GDriveStartStopType     kraken_file_get_start_stop_type               (KrakenFile                   *file);
gboolean                kraken_file_can_poll_for_media                (KrakenFile                   *file);
gboolean                kraken_file_is_media_check_automatic          (KrakenFile                   *file);

void                    kraken_file_mount                             (KrakenFile                   *file,
									 GMountOperation                *mount_op,
									 GCancellable                   *cancellable,
									 KrakenFileOperationCallback   callback,
									 gpointer                        callback_data);
void                    kraken_file_unmount                           (KrakenFile                   *file,
									 GMountOperation                *mount_op,
									 GCancellable                   *cancellable,
									 KrakenFileOperationCallback   callback,
									 gpointer                        callback_data);
void                    kraken_file_eject                             (KrakenFile                   *file,
									 GMountOperation                *mount_op,
									 GCancellable                   *cancellable,
									 KrakenFileOperationCallback   callback,
									 gpointer                        callback_data);

void                    kraken_file_start                             (KrakenFile                   *file,
									 GMountOperation                *start_op,
									 GCancellable                   *cancellable,
									 KrakenFileOperationCallback   callback,
									 gpointer                        callback_data);
void                    kraken_file_stop                              (KrakenFile                   *file,
									 GMountOperation                *mount_op,
									 GCancellable                   *cancellable,
									 KrakenFileOperationCallback   callback,
									 gpointer                        callback_data);
void                    kraken_file_poll_for_media                    (KrakenFile                   *file);

/* Basic operations for file objects. */
void                    kraken_file_set_owner                         (KrakenFile                   *file,
									 const char                     *user_name_or_id,
									 KrakenFileOperationCallback   callback,
									 gpointer                        callback_data);
void                    kraken_file_set_group                         (KrakenFile                   *file,
									 const char                     *group_name_or_id,
									 KrakenFileOperationCallback   callback,
									 gpointer                        callback_data);
void                    kraken_file_set_permissions                   (KrakenFile                   *file,
									 guint32                         permissions,
									 KrakenFileOperationCallback   callback,
									 gpointer                        callback_data);
void                    kraken_file_rename                            (KrakenFile                   *file,
									 const char                     *new_name,
									 KrakenFileOperationCallback   callback,
									 gpointer                        callback_data);
void                    kraken_file_cancel                            (KrakenFile                   *file,
									 KrakenFileOperationCallback   callback,
									 gpointer                        callback_data);

/* Return true if this file has already been deleted.
 * This object will be unref'd after sending the files_removed signal,
 * but it could hang around longer if someone ref'd it.
 */
gboolean                kraken_file_is_gone                           (KrakenFile                   *file);

/* Return true if this file is not confirmed to have ever really
 * existed. This is true when the KrakenFile object has been created, but no I/O
 * has yet confirmed the existence of a file by that name.
 */
gboolean                kraken_file_is_not_yet_confirmed              (KrakenFile                   *file);

/* Simple getting and setting top-level metadata. */
char *                  kraken_file_get_metadata                      (KrakenFile                   *file,
									 const char                     *key,
									 const char                     *default_metadata);
GList *                 kraken_file_get_metadata_list                 (KrakenFile                   *file,
									 const char                     *key);
void                    kraken_file_set_metadata                      (KrakenFile                   *file,
									 const char                     *key,
									 const char                     *default_metadata,
									 const char                     *metadata);
void                    kraken_file_set_metadata_list                 (KrakenFile                   *file,
									 const char                     *key,
									 GList                          *list);
void                    kraken_file_set_desktop_grid_adjusts (KrakenFile   *file,
                                                            const char *key,
                                                            int         int_a,
                                                            int         int_b);
void                    kraken_file_get_desktop_grid_adjusts (KrakenFile   *file,
                                                            const char *key,
                                                            int        *int_a,
                                                            int        *int_b);
/* Covers for common data types. */
gboolean                kraken_file_get_boolean_metadata              (KrakenFile                   *file,
									 const char                     *key,
									 gboolean                        default_metadata);
void                    kraken_file_set_boolean_metadata              (KrakenFile                   *file,
									 const char                     *key,
									 gboolean                        default_metadata,
									 gboolean                        metadata);
int                     kraken_file_get_integer_metadata              (KrakenFile                   *file,
									 const char                     *key,
									 int                             default_metadata);
void                    kraken_file_set_integer_metadata              (KrakenFile                   *file,
									 const char                     *key,
									 int                             default_metadata,
									 int                             metadata);

#define UNDEFINED_TIME ((time_t) (-1))

time_t                  kraken_file_get_time_metadata                 (KrakenFile                  *file,
									 const char                    *key);
void                    kraken_file_set_time_metadata                 (KrakenFile                  *file,
									 const char                    *key,
									 time_t                         time);


/* Attributes for file objects as user-displayable strings. */
char *                  kraken_file_get_string_attribute              (KrakenFile                   *file,
									 const char                     *attribute_name);
char *                  kraken_file_get_string_attribute_q            (KrakenFile                   *file,
									 GQuark                          attribute_q);
char *                  kraken_file_get_string_attribute_with_default (KrakenFile                   *file,
									 const char                     *attribute_name);
char *                  kraken_file_get_string_attribute_with_default_q (KrakenFile                  *file,
									 GQuark                          attribute_q);

/* Matching with another URI. */
gboolean                kraken_file_matches_uri                       (KrakenFile                   *file,
									 const char                     *uri);

/* Is the file local? */
gboolean                kraken_file_is_local                          (KrakenFile                   *file);

/* Comparing two file objects for sorting */
KrakenFileSortType    kraken_file_get_default_sort_type             (KrakenFile                   *file,
									 gboolean                       *reversed);
const gchar *           kraken_file_get_default_sort_attribute        (KrakenFile                   *file,
									 gboolean                       *reversed);

int                     kraken_file_compare_for_sort                  (KrakenFile                   *file_1,
									 KrakenFile                   *file_2,
									 KrakenFileSortType            sort_type,
									 gboolean			 directories_first,
									 gboolean            favorites_first,
									 gboolean		  	 reversed,
                                     gpointer                       search_dir);
int                     kraken_file_compare_for_sort_by_attribute     (KrakenFile                   *file_1,
									 KrakenFile                   *file_2,
									 const char                     *attribute,
									 gboolean                        directories_first,
									 gboolean                        favorites_first,
									 gboolean                        reversed,
                                     gpointer                        search_dir);
int                     kraken_file_compare_for_sort_by_attribute_q   (KrakenFile                   *file_1,
									 KrakenFile                   *file_2,
									 GQuark                          attribute,
									 gboolean                        directories_first,
									 gboolean                        favorites_first,
									 gboolean                        reversed,
                                     gpointer                        search_dir);
gboolean                kraken_file_is_date_sort_attribute_q          (GQuark                          attribute);

int                     kraken_file_compare_display_name              (KrakenFile                   *file_1,
									 const char                     *pattern);
int                     kraken_file_compare_location                  (KrakenFile                    *file_1,
                                                                         KrakenFile                    *file_2);

/* filtering functions for use by various directory views */
gboolean                kraken_file_is_hidden_file                    (KrakenFile                   *file);
gboolean                kraken_file_should_show                       (KrakenFile                   *file,
									 gboolean                        show_hidden,
									 gboolean                        show_foreign);
GList                  *kraken_file_list_filter_hidden                (GList                          *files,
									 gboolean                        show_hidden);


/* Get the URI that's used when activating the file.
 * Getting this can require reading the contents of the file.
 */
gboolean                kraken_file_is_launcher                       (KrakenFile                   *file);
gboolean                kraken_file_is_foreign_link                   (KrakenFile                   *file);
gboolean                kraken_file_is_trusted_link                   (KrakenFile                   *file);
gboolean                kraken_file_has_activation_uri                (KrakenFile                   *file);
char *                  kraken_file_get_activation_uri                (KrakenFile                   *file);
GFile *                 kraken_file_get_activation_location           (KrakenFile                   *file);

char *                  kraken_file_get_drop_target_uri               (KrakenFile                   *file);

GIcon *                 kraken_file_get_gicon                         (KrakenFile                   *file,
                                                                     KrakenFileIconFlags           flags);
gchar *                 kraken_file_get_control_icon_name             (KrakenFile                   *file);

KrakenIconInfo *      kraken_file_get_icon                          (KrakenFile                   *file,
									 int                             size,
                                     int                             max_width,
                                     int                             scale,
									 KrakenFileIconFlags           flags);
GdkPixbuf *             kraken_file_get_icon_pixbuf                   (KrakenFile                   *file,
									 int                             size,
									 gboolean                        force_size,
                                     int                             scale,
									 KrakenFileIconFlags           flags);

gboolean                kraken_file_has_open_window                   (KrakenFile                   *file);
void                    kraken_file_set_has_open_window               (KrakenFile                   *file,
									 gboolean                        has_open_window);

/* Thumbnailing handling */
gboolean                kraken_file_is_thumbnailing                   (KrakenFile                   *file);

/* Convenience functions for dealing with a list of KrakenFile objects that each have a ref.
 * These are just convenient names for functions that work on lists of GtkObject *.
 */
GList *                 kraken_file_list_ref                          (GList                          *file_list);
void                    kraken_file_list_unref                        (GList                          *file_list);
void                    kraken_file_list_free                         (GList                          *file_list);
GList *                 kraken_file_list_copy                         (GList                          *file_list);
GList *                 kraken_file_list_from_uris                    (GList                          *uri_list);
GList *			kraken_file_list_sort_by_display_name		(GList				*file_list);
void                    kraken_file_list_call_when_ready              (GList                          *file_list,
									 KrakenFileAttributes          attributes,
									 KrakenFileListHandle        **handle,
									 KrakenFileListCallback        callback,
									 gpointer                        callback_data);
void                    kraken_file_list_cancel_call_when_ready       (KrakenFileListHandle         *handle);

char *   kraken_file_get_owner_as_string            (KrakenFile          *file,
                                                          gboolean           include_real_name);
char *   kraken_file_get_type_as_string             (KrakenFile          *file);
char *   kraken_file_get_detailed_type_as_string    (KrakenFile          *file);

gchar *  kraken_file_construct_tooltip              (KrakenFile *file, KrakenFileTooltipFlags flags, gpointer search_dir);

gboolean kraken_file_has_thumbnail_access_problem   (KrakenFile *file);

gint     kraken_file_get_monitor_number             (KrakenFile *file);
void     kraken_file_set_monitor_number             (KrakenFile *file, gint monitor);
void     kraken_file_get_position                   (KrakenFile *file, GdkPoint *point);
void     kraken_file_set_position                   (KrakenFile *file, gint x, gint y);
gboolean kraken_file_get_is_desktop_orphan          (KrakenFile *file);
void     kraken_file_set_is_desktop_orphan          (KrakenFile *file, gboolean is_desktop_orphan);

gboolean kraken_file_get_pinning                    (KrakenFile *file);
void     kraken_file_set_pinning                    (KrakenFile *file, gboolean  pin);
gboolean kraken_file_get_is_favorite                (KrakenFile *file);
void     kraken_file_set_is_favorite                (KrakenFile *file, gboolean favorite);
void     kraken_file_set_load_deferred_attrs        (KrakenFile *file,
                                                   KrakenFileLoadDeferredAttrs load_deferred_attrs);
KrakenFileLoadDeferredAttrs kraken_file_get_load_deferred_attrs (KrakenFile *file);

gboolean kraken_file_add_search_result_data             (KrakenFile *file, gpointer search_dir, FileSearchResult *result);
void kraken_file_clear_search_result_data           (KrakenFile *file, gpointer search_dir);
gboolean kraken_file_has_search_result              (KrakenFile *file, gpointer search_dir);
gint kraken_file_get_search_result_count            (KrakenFile *file, gpointer search_dir);
gchar *kraken_file_get_search_result_count_as_string (KrakenFile *file, gpointer search_dir);
gchar *kraken_file_get_search_result_snippet        (KrakenFile *file, gpointer search_dir);

/* Debugging */
void                    kraken_file_dump                              (KrakenFile                   *file);

typedef struct KrakenFileDetails KrakenFileDetails;

struct KrakenFile {
	GObject parent_slot;
	KrakenFileDetails *details;
};

typedef struct {
	GObjectClass parent_slot;

	/* Subclasses can set this to something other than G_FILE_TYPE_UNKNOWN and
	   it will be used as the default file type. This is useful when creating
	   a "virtual" KrakenFile subclass that you can't actually get real
	   information about. For exaple KrakenDesktopDirectoryFile. */
	GFileType default_file_type; 
	
	/* Called when the file notices any change. */
	void                  (* changed)                (KrakenFile *file);

	/* Called periodically while directory deep count is being computed. */
	void                  (* updated_deep_count_in_progress) (KrakenFile *file);

	/* Virtual functions (mainly used for trash directory). */
	void                  (* monitor_add)            (KrakenFile           *file,
							  gconstpointer           client,
							  KrakenFileAttributes  attributes);
	void                  (* monitor_remove)         (KrakenFile           *file,
							  gconstpointer           client);
	void                  (* call_when_ready)        (KrakenFile           *file,
							  KrakenFileAttributes  attributes,
							  KrakenFileCallback    callback,
							  gpointer                callback_data);
	void                  (* cancel_call_when_ready) (KrakenFile           *file,
							  KrakenFileCallback    callback,
							  gpointer                callback_data);
	gboolean              (* check_if_ready)         (KrakenFile           *file,
							  KrakenFileAttributes  attributes);
	gboolean              (* get_item_count)         (KrakenFile           *file,
							  guint                  *count,
							  gboolean               *count_unreadable);
	KrakenRequestStatus (* get_deep_counts)        (KrakenFile           *file,
							  guint                  *directory_count,
							  guint                  *file_count,
							  guint                  *unreadable_directory_count,
                              guint                  *hidden_count,
							  goffset       *total_size);
	gboolean              (* get_date)               (KrakenFile           *file,
							  KrakenDateType        type,
							  time_t                 *date);
	char *                (* get_where_string)       (KrakenFile           *file);

	void                  (* set_metadata)           (KrakenFile           *file,
                                                      const char         *key,
                                                      const char         *value);
	void                  (* set_metadata_as_list)   (KrakenFile           *file,
                                                      const char         *key,
                                                      char              **value);
    gchar *               (* get_metadata)           (KrakenFile           *file,
                                                      const char         *key);
    gchar **              (* get_metadata_as_list)   (KrakenFile           *file,
                                                      const char         *key);
	void                  (* mount)                  (KrakenFile                   *file,
							  GMountOperation                *mount_op,
							  GCancellable                   *cancellable,
							  KrakenFileOperationCallback   callback,
							  gpointer                        callback_data);
	void                 (* unmount)                 (KrakenFile                   *file,
							  GMountOperation                *mount_op,
							  GCancellable                   *cancellable,
							  KrakenFileOperationCallback   callback,
							  gpointer                        callback_data);
	void                 (* eject)                   (KrakenFile                   *file,
							  GMountOperation                *mount_op,
							  GCancellable                   *cancellable,
							  KrakenFileOperationCallback   callback,
							  gpointer                        callback_data);

	void                  (* start)                  (KrakenFile                   *file,
							  GMountOperation                *start_op,
							  GCancellable                   *cancellable,
							  KrakenFileOperationCallback   callback,
							  gpointer                        callback_data);
	void                 (* stop)                    (KrakenFile                   *file,
							  GMountOperation                *mount_op,
							  GCancellable                   *cancellable,
							  KrakenFileOperationCallback   callback,
							  gpointer                        callback_data);

	void                 (* poll_for_media)          (KrakenFile                   *file);
} KrakenFileClass;

#define KRAKEN_FILE_URI(msg,f)                                    \
    {                                                           \
        gchar *uri = kraken_file_get_uri (KRAKEN_FILE (f));         \
        g_message ("%s: %p - %s", msg, f, uri);                 \
        g_free (uri);                                           \
    }                                                           \

#endif /* KRAKEN_FILE_H */
