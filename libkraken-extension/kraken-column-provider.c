/*
 *  kraken-column-provider.c - Interface for Kraken extensions 
 *                               that provide column specifications.
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

#include <config.h>
#include "kraken-column-provider.h"

#include <glib-object.h>

G_DEFINE_INTERFACE (KrakenColumnProvider, kraken_column_provider, G_TYPE_OBJECT)

/**
 * SECTION:kraken-column-provider
 * @Title: KrakenColumnProvider
 * @Short_description: An interface to provide additional Kraken list view columns.
 *
 * This allows additional columns to be shown in the list view.  This interface
 * generally needs to be used in tandem with a #KrakenInfoProvider, to feed file
 * info back to populate the column(s).
 *
 **/

static void
kraken_column_provider_default_init (KrakenColumnProviderInterface *klass)
{
}

/**
 * kraken_column_provider_get_columns:
 * @provider: a #KrakenColumnProvider
 *
 * Returns: (element-type KrakenColumn) (transfer full): the provided #KrakenColumn objects
 **/
GList *
kraken_column_provider_get_columns (KrakenColumnProvider *provider)
{
	g_return_val_if_fail (KRAKEN_IS_COLUMN_PROVIDER (provider), NULL);
	g_return_val_if_fail (KRAKEN_COLUMN_PROVIDER_GET_IFACE (provider)->get_columns != NULL, NULL);

	return KRAKEN_COLUMN_PROVIDER_GET_IFACE (provider)->get_columns (provider);
}

