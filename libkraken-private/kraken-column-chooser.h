/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* kraken-column-choose.h - A column chooser widget

   Copyright (C) 2004 Novell, Inc.

   The Gnome Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The Gnome Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the column COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 51 Franklin Street - Suite 500,
   Boston, MA 02110-1335, USA.

   Authors: Dave Camp <dave@ximian.com>
*/

#ifndef KRAKEN_COLUMN_CHOOSER_H
#define KRAKEN_COLUMN_CHOOSER_H

#include <gtk/gtk.h>
#include <libkraken-private/kraken-file.h>

#define KRAKEN_TYPE_COLUMN_CHOOSER kraken_column_chooser_get_type()
#define KRAKEN_COLUMN_CHOOSER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_COLUMN_CHOOSER, KrakenColumnChooser))
#define KRAKEN_COLUMN_CHOOSER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_COLUMN_CHOOSER, KrakenColumnChooserClass))
#define KRAKEN_IS_COLUMN_CHOOSER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_COLUMN_CHOOSER))
#define KRAKEN_IS_COLUMN_CHOOSER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_COLUMN_CHOOSER))
#define KRAKEN_COLUMN_CHOOSER_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_COLUMN_CHOOSER, KrakenColumnChooserClass))

typedef struct _KrakenColumnChooserDetails KrakenColumnChooserDetails;

typedef struct {
	GtkBox parent;
	
	KrakenColumnChooserDetails *details;
} KrakenColumnChooser;

typedef struct {
        GtkBoxClass parent_slot;

	void (*changed) (KrakenColumnChooser *chooser);
	void (*use_default) (KrakenColumnChooser *chooser);
} KrakenColumnChooserClass;

GType      kraken_column_chooser_get_type            (void);
GtkWidget *kraken_column_chooser_new                 (KrakenFile *file);
void       kraken_column_chooser_set_settings    (KrakenColumnChooser   *chooser,
						    char                   **visible_columns, 
						    char                   **column_order);
void       kraken_column_chooser_get_settings    (KrakenColumnChooser *chooser,
						    char                  ***visible_columns, 
						    char                  ***column_order);

#endif /* KRAKEN_COLUMN_CHOOSER_H */
