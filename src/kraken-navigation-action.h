/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */

/*
 *  Kraken
 *
 *  Copyright (C) 2004 Red Hat, Inc.
 *  Copyright (C) 2003 Marco Pesenti Gritti
 *
 *  Kraken is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  Kraken is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Suite 500, MA 02110-1335, USA.
 *
 *
 *  Based on ephy-navigation-action.h from Epiphany
 *
 *  Authors: Alexander Larsson <alexl@redhat.com>
 *           Marco Pesenti Gritti
 *
 */

#ifndef KRAKEN_NAVIGATION_ACTION_H
#define KRAKEN_NAVIGATION_ACTION_H

#include <gtk/gtk.h>

#define KRAKEN_TYPE_NAVIGATION_ACTION            (kraken_navigation_action_get_type ())
#define KRAKEN_NAVIGATION_ACTION(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_NAVIGATION_ACTION, KrakenNavigationAction))
#define KRAKEN_NAVIGATION_ACTION_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_NAVIGATION_ACTION, KrakenNavigationActionClass))
#define KRAKEN_IS_NAVIGATION_ACTION(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_NAVIGATION_ACTION))
#define KRAKEN_IS_NAVIGATION_ACTION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((obj), KRAKEN_TYPE_NAVIGATION_ACTION))
#define KRAKEN_NAVIGATION_ACTION_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), KRAKEN_TYPE_NAVIGATION_ACTION, KrakenNavigationActionClass))

typedef struct _KrakenNavigationAction       KrakenNavigationAction;
typedef struct _KrakenNavigationActionClass  KrakenNavigationActionClass;
typedef struct KrakenNavigationActionPrivate KrakenNavigationActionPrivate;

typedef enum
{
    KRAKEN_NAVIGATION_DIRECTION_BACK,
    KRAKEN_NAVIGATION_DIRECTION_FORWARD,
    KRAKEN_NAVIGATION_DIRECTION_UP,
    KRAKEN_NAVIGATION_DIRECTION_RELOAD,
    KRAKEN_NAVIGATION_DIRECTION_HOME,
    KRAKEN_NAVIGATION_DIRECTION_COMPUTER,
    KRAKEN_NAVIGATION_DIRECTION_EDIT,

} KrakenNavigationDirection;

struct _KrakenNavigationAction
{
	GtkAction parent;
	
	/*< private >*/
	KrakenNavigationActionPrivate *priv;
};

struct _KrakenNavigationActionClass
{
	GtkActionClass parent_class;
};

GType    kraken_navigation_action_get_type   (void);

#endif
