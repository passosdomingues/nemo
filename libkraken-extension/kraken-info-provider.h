/*
 *  kraken-info-provider.h - Interface for Kraken extensions that 
 *                             provide info about files.
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
 * provide information about files.  Extensions are called when Kraken 
 * needs information about a file.  They are passed a KrakenFileInfo 
 * object which should be filled with relevant information */

#ifndef KRAKEN_INFO_PROVIDER_H
#define KRAKEN_INFO_PROVIDER_H

#include <glib-object.h>
#include "kraken-extension-types.h"
#include "kraken-file-info.h"

G_BEGIN_DECLS

#define KRAKEN_TYPE_INFO_PROVIDER           (kraken_info_provider_get_type ())

G_DECLARE_INTERFACE (KrakenInfoProvider, kraken_info_provider,
                     KRAKEN, INFO_PROVIDER,
                     GObject)

typedef KrakenInfoProviderInterface KrakenInfoProviderIface;

typedef void (*KrakenInfoProviderUpdateComplete) (KrakenInfoProvider    *provider,
						    KrakenOperationHandle *handle,
						    KrakenOperationResult  result,
						    gpointer                 user_data);

struct _KrakenInfoProviderInterface {
	GTypeInterface g_iface;

	KrakenOperationResult (*update_file_info) (KrakenInfoProvider     *provider,
						     KrakenFileInfo         *file,
						     GClosure                 *update_complete,
						     KrakenOperationHandle **handle);
	void                    (*cancel_update)    (KrakenInfoProvider     *provider,
						     KrakenOperationHandle  *handle);
};

/* pre-G_DECLARE_INTERFACE/G_DEFINE_INTERFACE compatibility */
#define KrakenInfoProviderIface KrakenInfoProviderInterface

/* Interface Functions */
KrakenOperationResult kraken_info_provider_update_file_info       (KrakenInfoProvider     *provider,
								       KrakenFileInfo         *file,
								       GClosure                 *update_complete,
								       KrakenOperationHandle **handle);
void                    kraken_info_provider_cancel_update          (KrakenInfoProvider     *provider,
								       KrakenOperationHandle  *handle);

/* Helper functions for implementations */
void                    kraken_info_provider_update_complete_invoke (GClosure                 *update_complete,
								       KrakenInfoProvider     *provider,
								       KrakenOperationHandle  *handle,
								       KrakenOperationResult   result);

G_END_DECLS

#endif
