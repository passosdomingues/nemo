/*
 * kraken-previewer: kraken previewer DBus wrapper
 *
 * Copyright (C) 2011, Red Hat, Inc.
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Suite 500, MA 02110-1335, USA.
 *
 * Author: Cosimo Cecchi <cosimoc@redhat.com>
 *
 */

#ifndef __KRAKEN_PREVIEWER_H__
#define __KRAKEN_PREVIEWER_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define KRAKEN_TYPE_PREVIEWER kraken_previewer_get_type()
#define KRAKEN_PREVIEWER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_PREVIEWER, KrakenPreviewer))
#define KRAKEN_PREVIEWER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_PREVIEWER, KrakenPreviewerClass))
#define KRAKEN_IS_PREVIEWER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_PREVIEWER))
#define KRAKEN_IS_PREVIEWER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_PREVIEWER))
#define KRAKEN_PREVIEWER_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_PREVIEWER, KrakenPreviewerClass))

typedef struct _KrakenPreviewerPriv KrakenPreviewerPriv;

typedef struct {
  GObject parent;

  /* private */
  KrakenPreviewerPriv *priv;
} KrakenPreviewer;

typedef struct {
  GObjectClass parent_class;
} KrakenPreviewerClass;

GType kraken_previewer_get_type (void);

KrakenPreviewer *kraken_previewer_get_singleton (void);
void kraken_previewer_call_show_file (KrakenPreviewer *previewer,
                                        const gchar *uri,
                                        guint xid,
					gboolean close_if_already_visible);
void kraken_previewer_call_close (KrakenPreviewer *previewer);

G_END_DECLS

#endif /* __KRAKEN_PREVIEWER_H__ */
