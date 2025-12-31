/*
 *  kraken-property-page-provider.h - Interface for Kraken extensions
 *                                      that provide property pages.
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
 * add property page to property dialogs.  Extensions are called when 
 * Kraken needs property pages for a selection.  They are passed a 
 * list of KrakenFileInfo objects for which information should
 * be displayed  */

#ifndef KRAKEN_PROPERTY_PAGE_PROVIDER_H
#define KRAKEN_PROPERTY_PAGE_PROVIDER_H

#include <glib-object.h>
#include "kraken-extension-types.h"
#include "kraken-file-info.h"
#include "kraken-property-page.h"

G_BEGIN_DECLS

#define KRAKEN_TYPE_PROPERTY_PAGE_PROVIDER           (kraken_property_page_provider_get_type ())

G_DECLARE_INTERFACE (KrakenPropertyPageProvider, kraken_property_page_provider,
                     KRAKEN, PROPERTY_PAGE_PROVIDER,
                     GObject)

typedef KrakenPropertyPageProviderInterface KrakenPropertyPageProviderIface;

struct _KrakenPropertyPageProviderInterface {
	GTypeInterface g_iface;

	GList *(*get_pages) (KrakenPropertyPageProvider     *provider,
			     GList                    *files);
};

/* Interface Functions */
GList                  *kraken_property_page_provider_get_pages (KrakenPropertyPageProvider *provider,
								   GList                        *files);

G_END_DECLS

#endif
