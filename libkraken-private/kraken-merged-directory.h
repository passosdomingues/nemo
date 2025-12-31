/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   kraken-merged-directory.h: Subclass of KrakenDirectory to implement
   a virtual directory consisting of the merged contents of some real
   directories.
 
   Copyright (C) 1999, 2000 Eazel, Inc.
  
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

#ifndef KRAKEN_MERGED_DIRECTORY_H
#define KRAKEN_MERGED_DIRECTORY_H

#include <libkraken-private/kraken-directory.h>

#define KRAKEN_TYPE_MERGED_DIRECTORY kraken_merged_directory_get_type()
#define KRAKEN_MERGED_DIRECTORY(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_MERGED_DIRECTORY, KrakenMergedDirectory))
#define KRAKEN_MERGED_DIRECTORY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_MERGED_DIRECTORY, KrakenMergedDirectoryClass))
#define KRAKEN_IS_MERGED_DIRECTORY(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_MERGED_DIRECTORY))
#define KRAKEN_IS_MERGED_DIRECTORY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_MERGED_DIRECTORY))
#define KRAKEN_MERGED_DIRECTORY_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_MERGED_DIRECTORY, KrakenMergedDirectoryClass))

typedef struct KrakenMergedDirectoryDetails KrakenMergedDirectoryDetails;

typedef struct {
	KrakenDirectory parent_slot;
	KrakenMergedDirectoryDetails *details;
} KrakenMergedDirectory;

typedef struct {
	KrakenDirectoryClass parent_slot;

	void (* add_real_directory)    (KrakenMergedDirectory *merged_directory,
					KrakenDirectory       *real_directory);
	void (* remove_real_directory) (KrakenMergedDirectory *merged_directory,
					KrakenDirectory       *real_directory);
} KrakenMergedDirectoryClass;

GType   kraken_merged_directory_get_type              (void);
void    kraken_merged_directory_add_real_directory    (KrakenMergedDirectory *merged_directory,
							 KrakenDirectory       *real_directory);
void    kraken_merged_directory_remove_real_directory (KrakenMergedDirectory *merged_directory,
							 KrakenDirectory       *real_directory);
GList * kraken_merged_directory_get_real_directories  (KrakenMergedDirectory *merged_directory);

#endif /* KRAKEN_MERGED_DIRECTORY_H */
