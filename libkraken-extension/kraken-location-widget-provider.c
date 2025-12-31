/*
 *  kraken-location-widget-provider.c - Interface for Kraken
                 extensions that provide extra widgets for a location
 *
 *  Copyright (C) 2005 Red Hat, Inc.
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
 *  Author:  Alexander Larsson <alexl@redhat.com>
 *
 */

#include <config.h>
#include "kraken-location-widget-provider.h"

#include <glib-object.h>


G_DEFINE_INTERFACE (KrakenLocationWidgetProvider, kraken_location_widget_provider, G_TYPE_OBJECT)

/**
 * SECTION:kraken-location-widget-provider
 * @Title: KrakenLocationWidgetProvider
 * @Short_description: Allows a custom widget to be added to a Kraken view.

 * This is an interface to allow the provision of a custom location widget 
 * embedded at the top of the Kraken view.  It receives the current location, and
 * can then determine whether or not the location is appropriate for a widget, and
 * its contents.
 *
 * Be aware that this extension is queried for a new widget any time a view loads a
 * new location, or reloads the current one.
 **/

static void
kraken_location_widget_provider_default_init (KrakenLocationWidgetProviderInterface *klass)
{
}

/**
 * kraken_location_widget_provider_get_widget:
 * @provider: a #KrakenLocationWidgetProvider
 * @uri: the URI of the location
 * @window: parent #GtkWindow
 *
 * Returns: (transfer none): the location widget for @provider at @uri
 */
GtkWidget *
kraken_location_widget_provider_get_widget (KrakenLocationWidgetProvider     *provider,
					      const char                         *uri,
					      GtkWidget                          *window)
{
	g_return_val_if_fail (KRAKEN_IS_LOCATION_WIDGET_PROVIDER (provider), NULL);

	return KRAKEN_LOCATION_WIDGET_PROVIDER_GET_IFACE (provider)->get_widget 
		(provider, uri, window);

}				       
