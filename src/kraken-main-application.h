/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * kraken-application: main Kraken application class.
 *
 * Copyright (C) 2000 Red Hat, Inc.
 * Copyright (C) 2010 Cosimo Cecchi <cosimoc@gnome.org>
 *
 * Kraken is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * Kraken is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street - Suite 500,
 * Boston, MA 02110-1335, USA.
 */

#ifndef __KRAKEN_MAIN_APPLICATION_H__
#define __KRAKEN_MAIN_APPLICATION_H__

#include <gdk/gdk.h>
#include <gio/gio.h>
#include <gtk/gtk.h>

#include <libkraken-private/kraken-undo-manager.h>

#include "kraken-window.h"
#include "kraken-application.h"

#define KRAKEN_TYPE_MAIN_APPLICATION kraken_main_application_get_type()
#define KRAKEN_MAIN_APPLICATION(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_MAIN_APPLICATION, KrakenMainApplication))
#define KRAKEN_MAIN_APPLICATION_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_MAIN_APPLICATION, KrakenMainApplicationClass))
#define KRAKEN_IS_MAIN_APPLICATION(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_MAIN_APPLICATION))
#define KRAKEN_IS_MAIN_APPLICATION_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_MAIN_APPLICATION))
#define KRAKEN_MAIN_APPLICATION_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_MAIN_APPLICATION, KrakenMainApplicationClass))

typedef struct _KrakenMainApplicationPriv KrakenMainApplicationPriv;

typedef struct {
	KrakenApplication parent;

	KrakenMainApplicationPriv *priv;
} KrakenMainApplication;

typedef struct {
	KrakenApplicationClass parent_class;
} KrakenMainApplicationClass;

GType kraken_main_application_get_type (void);

KrakenApplication *kraken_main_application_get_singleton (void);

#endif /* __KRAKEN_MAIN_APPLICATION_H__ */
