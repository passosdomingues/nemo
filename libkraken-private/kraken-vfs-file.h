/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   kraken-vfs-file.h: Subclass of KrakenFile to implement the
   the case of a VFS file.
 
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

#ifndef KRAKEN_VFS_FILE_H
#define KRAKEN_VFS_FILE_H

#include <libkraken-private/kraken-file.h>

#define KRAKEN_TYPE_VFS_FILE kraken_vfs_file_get_type()
#define KRAKEN_VFS_FILE(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_VFS_FILE, KrakenVFSFile))
#define KRAKEN_VFS_FILE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_VFS_FILE, KrakenVFSFileClass))
#define KRAKEN_IS_VFS_FILE(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_VFS_FILE))
#define KRAKEN_IS_VFS_FILE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_VFS_FILE))
#define KRAKEN_VFS_FILE_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_VFS_FILE, KrakenVFSFileClass))

typedef struct KrakenVFSFileDetails KrakenVFSFileDetails;

typedef struct {
	KrakenFile parent_slot;
} KrakenVFSFile;

typedef struct {
	KrakenFileClass parent_slot;
} KrakenVFSFileClass;

GType   kraken_vfs_file_get_type (void);

#endif /* KRAKEN_VFS_FILE_H */
