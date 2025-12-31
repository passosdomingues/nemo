/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * kraken-progress-ui-handler.h: file operation progress user interface.
 *
 * Copyright (C) 2007, 2011 Red Hat, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street - Suite 500,
 * Boston, MA 02110-1335, USA.
 *
 * Authors: Alexander Larsson <alexl@redhat.com>
 *          Cosimo Cecchi <cosimoc@redhat.com>
 *
 */

#ifndef __KRAKEN_PROGRESS_UI_HANDLER_H__
#define __KRAKEN_PROGRESS_UI_HANDLER_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define KRAKEN_TYPE_PROGRESS_UI_HANDLER kraken_progress_ui_handler_get_type()
#define KRAKEN_PROGRESS_UI_HANDLER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_PROGRESS_UI_HANDLER, KrakenProgressUIHandler))
#define KRAKEN_PROGRESS_UI_HANDLER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_PROGRESS_UI_HANDLER, KrakenProgressUIHandlerClass))
#define KRAKEN_IS_PROGRESS_UI_HANDLER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_PROGRESS_UI_HANDLER))
#define KRAKEN_IS_PROGRESS_UI_HANDLER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_PROGRESS_UI_HANDLER))
#define KRAKEN_PROGRESS_UI_HANDLER_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_PROGRESS_UI_HANDLER, KrakenProgressUIHandlerClass))

typedef struct _KrakenProgressUIHandlerPriv KrakenProgressUIHandlerPriv;

typedef struct {
  GObject parent;

  /* private */
  KrakenProgressUIHandlerPriv *priv;
} KrakenProgressUIHandler;

typedef struct {
  GObjectClass parent_class;
} KrakenProgressUIHandlerClass;

GType kraken_progress_ui_handler_get_type (void);

KrakenProgressUIHandler * kraken_progress_ui_handler_new (void);

G_END_DECLS

#endif /* __KRAKEN_PROGRESS_UI_HANDLER_H__ */
