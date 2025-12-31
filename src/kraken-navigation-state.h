/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* Kraken - Kraken navigation state
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

#ifndef __KRAKEN_NAVIGATION_STATE_H__
#define __KRAKEN_NAVIGATION_STATE_H__

#include <glib-object.h>
#include <gtk/gtk.h>

#define KRAKEN_TYPE_NAVIGATION_STATE kraken_navigation_state_get_type()
#define KRAKEN_NAVIGATION_STATE(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_NAVIGATION_STATE, KrakenNavigationState))
#define KRAKEN_NAVIGATION_STATE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_NAVIGATION_STATE, KrakenNavigationStateClass))
#define KRAKEN_IS_NAVIGATION_STATE(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_NAVIGATION_STATE))
#define KRAKEN_IS_NAVIGATION_STATE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_NAVIGATION_STATE))
#define KRAKEN_NAVIGATION_STATE_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_NAVIGATION_STATE, KrakenNavigationStateClass))

typedef struct _KrakenNavigationState KrakenNavigationState;
typedef struct _KrakenNavigationStateClass KrakenNavigationStateClass;
typedef struct _KrakenNavigationStateDetails KrakenNavigationStateDetails;

struct _KrakenNavigationState {
	GObject parent;
	KrakenNavigationStateDetails *priv;
};

struct _KrakenNavigationStateClass {
	GObjectClass parent_class;
};

/* GObject */
GType       kraken_navigation_state_get_type  (void);

KrakenNavigationState * kraken_navigation_state_new (GtkActionGroup *slave,
                                                         const gchar **action_names);

void kraken_navigation_state_add_group (KrakenNavigationState *state,
                                          GtkActionGroup *group);
void kraken_navigation_state_set_master (KrakenNavigationState *state,
                                           GtkActionGroup *master);
GtkActionGroup * kraken_navigation_state_get_master (KrakenNavigationState *self);

void kraken_navigation_state_sync_all (KrakenNavigationState *state);

void kraken_navigation_state_set_boolean (KrakenNavigationState *self,
					    const gchar *action_name,
					    gboolean value);

#endif /* __KRAKEN_NAVIGATION_STATE_H__ */
