/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * Kraken
 *
 * Copyright (C) 2003 Red Hat, Inc.
 * Copyright (C) 2010 Cosimo Cecchi <cosimoc@gnome.org>
 *
 * Kraken is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * Kraken is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; see the file COPYING.  If not,
 * write to the Free Software Foundation, Inc., 51 Franklin Street - Suite 500,
 * Boston, MA 02110-1335, USA.
 */

#ifndef KRAKEN_CONNECT_SERVER_DIALOG_H
#define KRAKEN_CONNECT_SERVER_DIALOG_H

#include <gio/gio.h>
#include <gtk/gtk.h>

#include "kraken-application.h"
#include "kraken-window.h"

#define KRAKEN_TYPE_CONNECT_SERVER_DIALOG\
	(kraken_connect_server_dialog_get_type ())
#define KRAKEN_CONNECT_SERVER_DIALOG(obj)\
        (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_CONNECT_SERVER_DIALOG,\
				     KrakenConnectServerDialog))
#define KRAKEN_CONNECT_SERVER_DIALOG_CLASS(klass)\
	(G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_CONNECT_SERVER_DIALOG,\
				  KrakenConnectServerDialogClass))
#define KRAKEN_IS_CONNECT_SERVER_DIALOG(obj)\
	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_CONNECT_SERVER_DIALOG)

typedef struct _KrakenConnectServerDialog KrakenConnectServerDialog;
typedef struct _KrakenConnectServerDialogClass KrakenConnectServerDialogClass;
typedef struct _KrakenConnectServerDialogDetails KrakenConnectServerDialogDetails;

struct _KrakenConnectServerDialog {
	GtkDialog parent;
	KrakenConnectServerDialogDetails *details;
};

struct _KrakenConnectServerDialogClass {
	GtkDialogClass parent_class;
};

GType kraken_connect_server_dialog_get_type (void);

GtkWidget* kraken_connect_server_dialog_new (KrakenWindow *window);

void kraken_connect_server_dialog_display_location_async (KrakenConnectServerDialog *self,
							    GFile *location,
							    GAsyncReadyCallback callback,
							    gpointer user_data);
gboolean kraken_connect_server_dialog_display_location_finish (KrakenConnectServerDialog *self,
								 GAsyncResult *result,
								 GError **error);

void kraken_connect_server_dialog_fill_details_async (KrakenConnectServerDialog *self,
							GMountOperation *operation,
							const gchar *default_user,
							const gchar *default_domain,
							GAskPasswordFlags flags,
							GAsyncReadyCallback callback,
							gpointer user_data);
gboolean kraken_connect_server_dialog_fill_details_finish (KrakenConnectServerDialog *self,
							     GAsyncResult *result);

#endif /* KRAKEN_CONNECT_SERVER_DIALOG_H */
