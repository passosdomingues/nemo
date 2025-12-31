/*
 *  kraken-info-provider.h - Interface for Kraken extensions that 
 *                             provide info about files.
 *
 *  Copyright (C) 2003 Novell, Inc.
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
 *  Author:  Dave Camp <dave@ximian.com>
 *           Alexander Larsson <alexl@redhat.com>
 *
 */

/* This interface is implemented by Kraken extensions that want to 
 * provide extra location widgets for a particular location.
 * Extensions are called when Kraken displays a location.
 */

#ifndef KRAKEN_LOCATION_WIDGET_PROVIDER_H
#define KRAKEN_LOCATION_WIDGET_PROVIDER_H

#include <glib-object.h>
#include <gtk/gtk.h>
#include "kraken-extension-types.h"

G_BEGIN_DECLS

#define KRAKEN_TYPE_LOCATION_WIDGET_PROVIDER           (kraken_location_widget_provider_get_type ())

G_DECLARE_INTERFACE (KrakenLocationWidgetProvider, kraken_location_widget_provider,
                     KRAKEN, LOCATION_WIDGET_PROVIDER,
                     GObject)

typedef KrakenLocationWidgetProviderInterface KrakenLocationWidgetProviderIface;

struct _KrakenLocationWidgetProviderInterface {
	GTypeInterface g_iface;

	GtkWidget * (*get_widget) (KrakenLocationWidgetProvider *provider,
				   const char                     *uri,
				   GtkWidget                      *window);
};

/* Interface Functions */
GtkWidget * kraken_location_widget_provider_get_widget    (KrakenLocationWidgetProvider     *provider,
							     const char                         *uri,
							     GtkWidget                          *window);

G_END_DECLS

#endif
