/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   kraken-search-directory.h: Subclass of KrakenDirectory to implement
   a virtual directory consisting of the search directory and the search
   icons
 
   Copyright (C) 2005 Novell, Inc
  
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
*/

#ifndef KRAKEN_SEARCH_DIRECTORY_H
#define KRAKEN_SEARCH_DIRECTORY_H

#include <libkraken-private/kraken-directory.h>
#include <libkraken-private/kraken-query.h>

#define KRAKEN_TYPE_SEARCH_DIRECTORY kraken_search_directory_get_type()
#define KRAKEN_SEARCH_DIRECTORY(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_SEARCH_DIRECTORY, KrakenSearchDirectory))
#define KRAKEN_SEARCH_DIRECTORY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_SEARCH_DIRECTORY, KrakenSearchDirectoryClass))
#define KRAKEN_IS_SEARCH_DIRECTORY(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_SEARCH_DIRECTORY))
#define KRAKEN_IS_SEARCH_DIRECTORY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_SEARCH_DIRECTORY))
#define KRAKEN_SEARCH_DIRECTORY_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_SEARCH_DIRECTORY, KrakenSearchDirectoryClass))

typedef struct KrakenSearchDirectoryDetails KrakenSearchDirectoryDetails;

typedef struct {
	KrakenDirectory parent_slot;
	KrakenSearchDirectoryDetails *details;
} KrakenSearchDirectory;

typedef struct {
	KrakenDirectoryClass parent_slot;
} KrakenSearchDirectoryClass;

GType   kraken_search_directory_get_type             (void);

char   *kraken_search_directory_generate_new_uri     (void);

KrakenSearchDirectory *kraken_search_directory_new_from_saved_search (const char *uri);

gboolean       kraken_search_directory_is_saved_search (KrakenSearchDirectory *search);
gboolean       kraken_search_directory_is_modified     (KrakenSearchDirectory *search);
void           kraken_search_directory_save_search     (KrakenSearchDirectory *search);
void           kraken_search_directory_save_to_file    (KrakenSearchDirectory *search,
							  const char              *save_file_uri);

KrakenQuery *kraken_search_directory_get_query       (KrakenSearchDirectory *search);
void           kraken_search_directory_set_query       (KrakenSearchDirectory *search,
							  KrakenQuery           *query);

#endif /* KRAKEN_SEARCH_DIRECTORY_H */
