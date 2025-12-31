#ifndef KRAKEN_SIDEBAR_PROVIDER_H
#define KRAKEN_SIDEBAR_PROVIDER_H

#include <glib-object.h>
#include <libkraken-extension/kraken-property-page-provider.h>

G_BEGIN_DECLS

#define KRAKEN_TYPE_SIDEBAR_PROVIDER (kraken_sidebar_provider_get_type())
G_DECLARE_FINAL_TYPE(KrakenSidebarProvider, kraken_sidebar_provider, KRAKEN, SIDEBAR_PROVIDER, GObject)

void kraken_sidebar_provider_register_type(GTypeModule* module);

G_END_DECLS

#endif /* KRAKEN_SIDEBAR_PROVIDER_H */
