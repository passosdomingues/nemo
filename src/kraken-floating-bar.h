/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* Kraken - Floating status bar.
 *
 * Copyright (C) 2011 Red Hat Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street - Suite 500,
 * Boston, MA 02110-1335, USA.
 *
 * Authors: Cosimo Cecchi <cosimoc@redhat.com>
 *
 */

#ifndef __KRAKEN_FLOATING_BAR_H__
#define __KRAKEN_FLOATING_BAR_H__

#include <gtk/gtk.h>

#define KRAKEN_FLOATING_BAR_ACTION_ID_STOP 1

#define KRAKEN_TYPE_FLOATING_BAR kraken_floating_bar_get_type()
#define KRAKEN_FLOATING_BAR(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_FLOATING_BAR, KrakenFloatingBar))
#define KRAKEN_FLOATING_BAR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_FLOATING_BAR, KrakenFloatingBarClass))
#define KRAKEN_IS_FLOATING_BAR(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_FLOATING_BAR))
#define KRAKEN_IS_FLOATING_BAR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_FLOATING_BAR))
#define KRAKEN_FLOATING_BAR_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_FLOATING_BAR, KrakenFloatingBarClass))

typedef struct _KrakenFloatingBar KrakenFloatingBar;
typedef struct _KrakenFloatingBarClass KrakenFloatingBarClass;
typedef struct _KrakenFloatingBarDetails KrakenFloatingBarDetails;

struct _KrakenFloatingBar {
	GtkBox parent;
	KrakenFloatingBarDetails *priv;
};

struct _KrakenFloatingBarClass {
	GtkBoxClass parent_class;
};

/* GObject */
GType       kraken_floating_bar_get_type  (void);

GtkWidget * kraken_floating_bar_new              (const gchar *label,
						    gboolean show_spinner);

void        kraken_floating_bar_set_label        (KrakenFloatingBar *self,
						    const gchar *label);
void        kraken_floating_bar_set_show_spinner (KrakenFloatingBar *self,
						    gboolean show_spinner);

void        kraken_floating_bar_add_action       (KrakenFloatingBar *self,
						    const gchar *stock_id,
						    gint action_id);
void        kraken_floating_bar_cleanup_actions  (KrakenFloatingBar *self);

#endif /* __KRAKEN_FLOATING_BAR_H__ */

