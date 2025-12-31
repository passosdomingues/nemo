/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * Kraken
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

#ifndef __KRAKEN_TOOLBAR_H__
#define __KRAKEN_TOOLBAR_H__

#include <gtk/gtk.h>

#define KRAKEN_TYPE_TOOLBAR kraken_toolbar_get_type()
#define KRAKEN_TOOLBAR(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_TOOLBAR, KrakenToolbar))
#define KRAKEN_TOOLBAR_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_TOOLBAR, KrakenToolbarClass))
#define KRAKEN_IS_TOOLBAR(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_TOOLBAR))
#define KRAKEN_IS_TOOLBAR_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_TOOLBAR))
#define KRAKEN_TOOLBAR_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_TOOLBAR, KrakenToolbarClass))

typedef struct _KrakenToolbar KrakenToolbar;
typedef struct _KrakenToolbarPriv KrakenToolbarPriv;
typedef struct _KrakenToolbarClass KrakenToolbarClass;

typedef enum {
	KRAKEN_TOOLBAR_MODE_PATH_BAR,
	KRAKEN_TOOLBAR_MODE_LOCATION_BAR,
} KrakenToolbarMode;

struct _KrakenToolbar {
	GtkBox parent;

	/* private */
	KrakenToolbarPriv *priv;
};

struct _KrakenToolbarClass {
	GtkBoxClass parent_class;
};

GType kraken_toolbar_get_type (void);

GtkWidget *kraken_toolbar_new (GtkActionGroup *action_group);

gboolean  kraken_toolbar_get_show_location_entry (KrakenToolbar *self);
GtkWidget *kraken_toolbar_get_path_bar (KrakenToolbar *self);
GtkWidget *kraken_toolbar_get_location_bar (KrakenToolbar *self);

void kraken_toolbar_set_show_main_bar (KrakenToolbar *self,
					 gboolean show_main_bar);
void kraken_toolbar_set_show_location_entry (KrakenToolbar *self,
					       gboolean show_location_entry);
void kraken_toolbar_update_for_location (KrakenToolbar *self);
#endif /* __KRAKEN_TOOLBAR_H__ */
