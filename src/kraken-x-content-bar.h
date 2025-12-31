/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * Copyright (C) 2008 Red Hat, Inc.
 * Copyright (C) 2006 Paolo Borelli <pborelli@katamail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street - Suite 500, Boston, MA 02110-1335, USA.
 *
 * Authors: David Zeuthen <davidz@redhat.com>
 *          Paolo Borelli <pborelli@katamail.com>
 *
 */

#ifndef __KRAKEN_X_CONTENT_BAR_H
#define __KRAKEN_X_CONTENT_BAR_H

#include <gtk/gtk.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define KRAKEN_TYPE_X_CONTENT_BAR         (kraken_x_content_bar_get_type ())
#define KRAKEN_X_CONTENT_BAR(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), KRAKEN_TYPE_X_CONTENT_BAR, KrakenXContentBar))
#define KRAKEN_X_CONTENT_BAR_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), KRAKEN_TYPE_X_CONTENT_BAR, KrakenXContentBarClass))
#define KRAKEN_IS_X_CONTENT_BAR(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), KRAKEN_TYPE_X_CONTENT_BAR))
#define KRAKEN_IS_X_CONTENT_BAR_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), KRAKEN_TYPE_X_CONTENT_BAR))
#define KRAKEN_X_CONTENT_BAR_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), KRAKEN_TYPE_X_CONTENT_BAR, KrakenXContentBarClass))

typedef struct KrakenXContentBarPrivate KrakenXContentBarPrivate;

typedef struct
{
	GtkInfoBar parent;

	KrakenXContentBarPrivate *priv;
} KrakenXContentBar;

typedef struct
{
	GtkInfoBarClass parent_class;
} KrakenXContentBarClass;

GType		 kraken_x_content_bar_get_type	(void) G_GNUC_CONST;

GtkWidget	*kraken_x_content_bar_new		   (GMount              *mount, 
							    const char          *x_content_type);

G_END_DECLS

#endif /* __KRAKEN_X_CONTENT_BAR_H */
