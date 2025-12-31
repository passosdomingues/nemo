/*
 *  kraken-name-and-desc-provider.c - Interface for Kraken extensions that 
 *  returns the extension's proper name and description for the plugin
 *  manager only - it is not necessary for extension functionality.
 *
 */

#include <config.h>
#include "kraken-name-and-desc-provider.h"

#include <glib-object.h>

static void
kraken_name_and_desc_provider_default_init (KrakenNameAndDescProviderInterface *klass)
{
}

G_DEFINE_INTERFACE (KrakenNameAndDescProvider, kraken_name_and_desc_provider, G_TYPE_OBJECT)

/**
 * kraken_name_and_desc_provider_get_name_and_desc:
 * @provider: a #KrakenNameAndDescProvider
 *
 * Returns: (element-type gchar) (transfer full): a list of name:::desc
 * strings.  Optionally, the name of a path executable can be appended as a
 * third component of the list ('name:::desc:::foo-bar-preferences')
 */
GList *
kraken_name_and_desc_provider_get_name_and_desc (KrakenNameAndDescProvider *provider)
{
	g_return_val_if_fail (KRAKEN_IS_NAME_AND_DESC_PROVIDER (provider), NULL);
	g_return_val_if_fail (KRAKEN_NAME_AND_DESC_PROVIDER_GET_IFACE (provider)->get_name_and_desc != NULL, NULL);

	return KRAKEN_NAME_AND_DESC_PROVIDER_GET_IFACE (provider)->get_name_and_desc (provider);
}

