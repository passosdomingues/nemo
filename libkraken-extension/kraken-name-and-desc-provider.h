/*
 *  kraken-name-and-desc-provider.h - Interface for Kraken extensions that 
 *  returns the extension's proper name and description for the plugin
 *  manager only - it is not necessary for extension functionality.
 *
 */

#ifndef KRAKEN_NAME_AND_DESC_PROVIDER_H
#define KRAKEN_NAME_AND_DESC_PROVIDER_H

#include <glib-object.h>
#include "kraken-extension-types.h"

G_BEGIN_DECLS

#define KRAKEN_TYPE_NAME_AND_DESC_PROVIDER (kraken_name_and_desc_provider_get_type ())

G_DECLARE_INTERFACE (KrakenNameAndDescProvider, kraken_name_and_desc_provider,
                     KRAKEN, NAME_AND_DESC_PROVIDER,
                     GObject)

typedef KrakenNameAndDescProviderInterface KrakenNameAndDescProviderIface;

struct _KrakenNameAndDescProviderInterface {
    GTypeInterface g_iface;

    GList *(*get_name_and_desc) (KrakenNameAndDescProvider *provider);
};

/* Interface Functions */
GList *kraken_name_and_desc_provider_get_name_and_desc (KrakenNameAndDescProvider *provider);

G_END_DECLS

#endif
