/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * Kraken
 *
 * Copyright (C) 2000 Eazel, Inc.
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
 * Author: Maciej Stachowiak <mjs@eazel.com>
 *         Ettore Perazzoli <ettore@gnu.org>
 */

/* kraken-location-bar.h - Location bar for Kraken
 */

#ifndef KRAKEN_LOCATION_BAR_H
#define KRAKEN_LOCATION_BAR_H

#include <libkraken-private/kraken-entry.h>
#include <gtk/gtk.h>

#define KRAKEN_TYPE_LOCATION_BAR kraken_location_bar_get_type()
#define KRAKEN_LOCATION_BAR(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_LOCATION_BAR, KrakenLocationBar))
#define KRAKEN_LOCATION_BAR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_LOCATION_BAR, KrakenLocationBarClass))
#define KRAKEN_IS_LOCATION_BAR(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_LOCATION_BAR))
#define KRAKEN_IS_LOCATION_BAR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_LOCATION_BAR))
#define KRAKEN_LOCATION_BAR_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_LOCATION_BAR, KrakenLocationBarClass))

typedef struct KrakenLocationBarDetails KrakenLocationBarDetails;

typedef struct KrakenLocationBar {
	GtkBox parent;
	KrakenLocationBarDetails *details;
} KrakenLocationBar;

typedef struct {
	GtkBoxClass parent_class;

	/* for GtkBindingSet */
	void         (* cancel)           (KrakenLocationBar *bar);
} KrakenLocationBarClass;

GType      kraken_location_bar_get_type     	(void);
GtkWidget* kraken_location_bar_new          	(void);
KrakenEntry * kraken_location_bar_get_entry (KrakenLocationBar *location_bar);

void	kraken_location_bar_activate	 (KrakenLocationBar *bar);
void    kraken_location_bar_set_location     (KrakenLocationBar *bar,
						const char          *location);
gboolean kraken_location_bar_has_focus (KrakenLocationBar *location_bar);

#endif /* KRAKEN_LOCATION_BAR_H */
