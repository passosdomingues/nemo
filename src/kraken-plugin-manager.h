/* kraken-plugin-manager.h */

/*  A GtkWidget that can be inserted into a UI that provides a simple interface for
 *  managing the loading of extensions, actions and scripts
 */

#ifndef __KRAKEN_PLUGIN_MANAGER_H__
#define __KRAKEN_PLUGIN_MANAGER_H__

#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define KRAKEN_TYPE_PLUGIN_MANAGER (kraken_plugin_manager_get_type())

G_DECLARE_FINAL_TYPE (KrakenPluginManager, kraken_plugin_manager, KRAKEN, PLUGIN_MANAGER, GtkBin)

KrakenPluginManager       *kraken_plugin_manager_new                   (void);

G_END_DECLS

#endif /* __KRAKEN_PLUGIN_MANAGER_H__ */
