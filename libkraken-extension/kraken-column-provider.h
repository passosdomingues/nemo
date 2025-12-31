/*
 *  kraken-column-provider.h - Interface for Kraken extensions that 
 *                               provide column descriptions.
 *
 *  Copyright (C) 2003 Novell, Inc.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin Street, Suite 500, MA 02110-1335, USA.
 * 
 *  Author:  Dave Camp <dave@ximian.com>
 *
 */

/* This interface is implemented by Kraken extensions that want to
 * add columns to the list view and details to the icon view.
 * Extensions are asked for a list of columns to display.  Each
 * returned column refers to a string attribute which can be filled in
 * by KrakenInfoProvider */

#ifndef KRAKEN_COLUMN_PROVIDER_H
#define KRAKEN_COLUMN_PROVIDER_H

#include <glib-object.h>
#include "kraken-extension-types.h"
#include "kraken-column.h"

G_BEGIN_DECLS

#define KRAKEN_TYPE_COLUMN_PROVIDER           (kraken_column_provider_get_type ())

G_DECLARE_INTERFACE (KrakenColumnProvider, kraken_column_provider,
                     KRAKEN, COLUMN_PROVIDER,
                     GObject)

typedef KrakenColumnProviderInterface KrakenColumnProviderIface;

/**
 * KrakenColumnProviderInterface:
 * @g_iface: the parent class
 * @get_columns: Fetch an array of @KrakenColumn
 */
struct _KrakenColumnProviderInterface {
    GTypeInterface g_iface;

    GList *(*get_columns) (KrakenColumnProvider *provider);
};

/* Interface Functions */
GList                  *kraken_column_provider_get_columns    (KrakenColumnProvider *provider);

G_END_DECLS

#endif
