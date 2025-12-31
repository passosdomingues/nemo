/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-
 
   Copyright (C) 2007 Martin Wehner
  
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

   Author: Martin Wehner <martin.wehner@gmail.com>
*/

#ifndef KRAKEN_CELL_RENDERER_DISK_H
#define KRAKEN_CELL_RENDERER_DISK_H

#include <gtk/gtk.h>

#define KRAKEN_TYPE_CELL_RENDERER_DISK kraken_cell_renderer_disk_get_type()
#define KRAKEN_CELL_RENDERER_DISK(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_CELL_RENDERER_DISK, KrakenCellRendererDisk))
#define KRAKEN_CELL_RENDERER_DISK_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_CELL_RENDERER_DISK, KrakenCellRendererDiskClass))
#define KRAKEN_IS_CELL_RENDERER_DISK(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_CELL_RENDERER_DISK))
#define KRAKEN_IS_CELL_RENDERER_DISK_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_CELL_RENDERER_DISK))
#define KRAKEN_CELL_RENDERER_DISK_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_CELL_RENDERER_DISK, KrakenCellRendererDiskClass))


typedef struct _KrakenCellRendererDisk KrakenCellRendererDisk;
typedef struct _KrakenCellRendererDiskClass KrakenCellRendererDiskClass;

struct _KrakenCellRendererDisk {
	GtkCellRendererText parent;
    gint disk_full_percent;
    gboolean show_disk_full_percent;
    GtkTextDirection direction;
};

struct _KrakenCellRendererDiskClass {
	GtkCellRendererTextClass parent_class;
};

GType		 kraken_cell_renderer_disk_get_type (void);
GtkCellRenderer *kraken_cell_renderer_disk_new      (void);

#endif /* KRAKEN_CELL_RENDERER_DISK_H */
