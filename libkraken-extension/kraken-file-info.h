/*
 *  kraken-file-info.h - Information about a file 
 *
 *  Copyright (C) 2003 Novell, Inc.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin Street, Suite 500, MA 02110-1335, USA.
 *
 */

/* KrakenFileInfo is an interface to the KrakenFile object.  It 
 * provides access to the asynchronous data in the KrakenFile.
 * Extensions are passed objects of this type for operations. */

#ifndef KRAKEN_FILE_INFO_H
#define KRAKEN_FILE_INFO_H

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define KRAKEN_TYPE_FILE_INFO           (kraken_file_info_get_type ())
#define KRAKEN_FILE_INFO(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_FILE_INFO, KrakenFileInfo))
#define KRAKEN_IS_FILE_INFO(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_FILE_INFO))
#define KRAKEN_FILE_INFO_GET_IFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), KRAKEN_TYPE_FILE_INFO, KrakenFileInfoInterface))

#ifndef KRAKEN_FILE_DEFINED
#define KRAKEN_FILE_DEFINED
/* Using KrakenFile for the vtable to make implementing this in 
 * KrakenFile easier */
typedef struct KrakenFile          KrakenFile;
#endif

typedef KrakenFile KrakenFileInfo;
typedef struct _KrakenFileInfoInterface KrakenFileInfoInterface;

struct _KrakenFileInfoInterface 
{
	GTypeInterface g_iface;

	gboolean          (*is_gone)              (KrakenFileInfo *file);
	
	char *            (*get_name)             (KrakenFileInfo *file);
	char *            (*get_uri)              (KrakenFileInfo *file);
	char *            (*get_parent_uri)       (KrakenFileInfo *file);
	char *            (*get_uri_scheme)       (KrakenFileInfo *file);
	
	char *            (*get_mime_type)        (KrakenFileInfo *file);
	gboolean          (*is_mime_type)         (KrakenFileInfo *file,
						   const char       *mime_Type);
	gboolean          (*is_directory)         (KrakenFileInfo *file);
	
	void              (*add_emblem)           (KrakenFileInfo *file,
						   const char       *emblem_name);
	char *            (*get_string_attribute) (KrakenFileInfo *file,
						   const char       *attribute_name);
	void              (*add_string_attribute) (KrakenFileInfo *file,
						   const char       *attribute_name,
						   const char       *value);
	void              (*invalidate_extension_info) (KrakenFileInfo *file);
	
	char *            (*get_activation_uri)   (KrakenFileInfo *file);

	GFileType         (*get_file_type)        (KrakenFileInfo *file);
	GFile *           (*get_location)         (KrakenFileInfo *file);
	GFile *           (*get_parent_location)  (KrakenFileInfo *file);
	KrakenFileInfo* (*get_parent_info)      (KrakenFileInfo *file);
	GMount *          (*get_mount)            (KrakenFileInfo *file);
	gboolean          (*can_write)            (KrakenFileInfo *file);
  
};

GList            *kraken_file_info_list_copy            (GList            *files);
void              kraken_file_info_list_free            (GList            *files);
GType             kraken_file_info_get_type             (void);

/* Return true if the file has been deleted */
gboolean          kraken_file_info_is_gone              (KrakenFileInfo *file);

/* Name and Location */
GFileType         kraken_file_info_get_file_type        (KrakenFileInfo *file);
GFile *           kraken_file_info_get_location         (KrakenFileInfo *file);
char *            kraken_file_info_get_name             (KrakenFileInfo *file);
char *            kraken_file_info_get_uri              (KrakenFileInfo *file);
char *            kraken_file_info_get_activation_uri   (KrakenFileInfo *file);
GFile *           kraken_file_info_get_parent_location  (KrakenFileInfo *file);
char *            kraken_file_info_get_parent_uri       (KrakenFileInfo *file);
GMount *          kraken_file_info_get_mount            (KrakenFileInfo *file);
char *            kraken_file_info_get_uri_scheme       (KrakenFileInfo *file);
/* It's not safe to call this recursively multiple times, as it works
 * only for files already cached by Kraken.
 */
KrakenFileInfo* kraken_file_info_get_parent_info      (KrakenFileInfo *file);

/* File Type */
char *            kraken_file_info_get_mime_type        (KrakenFileInfo *file);
gboolean          kraken_file_info_is_mime_type         (KrakenFileInfo *file,
							   const char       *mime_type);
gboolean          kraken_file_info_is_directory         (KrakenFileInfo *file);
gboolean          kraken_file_info_can_write            (KrakenFileInfo *file);


/* Modifying the KrakenFileInfo */
void              kraken_file_info_add_emblem           (KrakenFileInfo *file,
							   const char       *emblem_name);
char *            kraken_file_info_get_string_attribute (KrakenFileInfo *file,
							   const char       *attribute_name);
void              kraken_file_info_add_string_attribute (KrakenFileInfo *file,
							   const char       *attribute_name,
							   const char       *value);

/* Invalidating file info */
void              kraken_file_info_invalidate_extension_info (KrakenFileInfo *file);

KrakenFileInfo *kraken_file_info_lookup                (GFile *location);
KrakenFileInfo *kraken_file_info_create                (GFile *location);
KrakenFileInfo *kraken_file_info_lookup_for_uri        (const char *uri);
KrakenFileInfo *kraken_file_info_create_for_uri        (const char *uri);

G_END_DECLS

#endif
