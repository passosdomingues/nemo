/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * Kraken
 *
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
 *
 * Author: Cosimo Cecchi <cosimoc@gnome.org>
 */

#ifndef __KRAKEN_CONNECT_SERVER_OPERATION_H__
#define __KRAKEN_CONNECT_SERVER_OPERATION_H__

#include <gio/gio.h>
#include <gtk/gtk.h>

#include "kraken-connect-server-dialog.h"

#define KRAKEN_TYPE_CONNECT_SERVER_OPERATION\
	(kraken_connect_server_operation_get_type ())
#define KRAKEN_CONNECT_SERVER_OPERATION(obj)\
  (G_TYPE_CHECK_INSTANCE_CAST ((obj),\
			       KRAKEN_TYPE_CONNECT_SERVER_OPERATION,\
			       KrakenConnectServerOperation))
#define KRAKEN_CONNECT_SERVER_OPERATION_CLASS(klass)\
  (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_CONNECT_SERVER_OPERATION,\
			    KrakenConnectServerOperationClass))
#define KRAKEN_IS_CONNECT_SERVER_OPERATION(obj)\
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_CONNECT_SERVER_OPERATION)

typedef struct _KrakenConnectServerOperationDetails
  KrakenConnectServerOperationDetails;

typedef struct {
	GtkMountOperation parent;
	KrakenConnectServerOperationDetails *details;
} KrakenConnectServerOperation;

typedef struct {
	GtkMountOperationClass parent_class;
} KrakenConnectServerOperationClass;

GType kraken_connect_server_operation_get_type (void);

GMountOperation *
kraken_connect_server_operation_new (KrakenConnectServerDialog *dialog);


#endif /* __KRAKEN_CONNECT_SERVER_OPERATION_H__ */
