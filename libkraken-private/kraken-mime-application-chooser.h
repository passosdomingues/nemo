/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
   kraken-mime-application-chooser.c: Manages applications for mime types
 
   Copyright (C) 2004 Novell, Inc.
 
   The Gnome Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The Gnome Library is distributed in the hope that it will be useful,
   but APPLICATIONOUT ANY WARRANTY; applicationout even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along application the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 51 Franklin Street - Suite 500,
   Boston, MA 02110-1335, USA.

   Authors: Dave Camp <dave@novell.com>
*/

#ifndef KRAKEN_MIME_APPLICATION_CHOOSER_H
#define KRAKEN_MIME_APPLICATION_CHOOSER_H

#include <gtk/gtk.h>

#define KRAKEN_TYPE_MIME_APPLICATION_CHOOSER         (kraken_mime_application_chooser_get_type ())
#define KRAKEN_MIME_APPLICATION_CHOOSER(obj)         (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_MIME_APPLICATION_CHOOSER, KrakenMimeApplicationChooser))
#define KRAKEN_MIME_APPLICATION_CHOOSER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_MIME_APPLICATION_CHOOSER, KrakenMimeApplicationChooserClass))
#define KRAKEN_IS_MIME_APPLICATION_CHOOSER(obj)      (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_MIME_APPLICATION_CHOOSER)

typedef struct _KrakenMimeApplicationChooser        KrakenMimeApplicationChooser;
typedef struct _KrakenMimeApplicationChooserClass   KrakenMimeApplicationChooserClass;
typedef struct _KrakenMimeApplicationChooserDetails KrakenMimeApplicationChooserDetails;

struct _KrakenMimeApplicationChooser {
	GtkBox parent;
	KrakenMimeApplicationChooserDetails *details;
};

struct _KrakenMimeApplicationChooserClass {
	GtkBoxClass parent_class;
};

GType      kraken_mime_application_chooser_get_type (void);
GtkWidget * kraken_mime_application_chooser_new (const char *uri,
                                                    GList *files,
                                               const char *mime_type,
                                                GtkWidget *ok_button);
GAppInfo  *kraken_mime_application_chooser_get_info (KrakenMimeApplicationChooser *chooser);
const gchar *kraken_mime_application_chooser_get_uri (KrakenMimeApplicationChooser *chooser);

#endif /* KRAKEN_MIME_APPLICATION_CHOOSER_H */
