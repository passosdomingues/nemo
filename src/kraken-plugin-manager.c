/* kraken-plugin-manager.c */

/*  A GtkWidget that can be inserted into a UI that provides a simple interface for
 *  managing the loading of extensions, actions and scripts
 */

#include <config.h>
#include "kraken-plugin-manager.h"
#include "kraken-action-config-widget.h"
#include "kraken-template-config-widget.h"
#include "kraken-extension-config-widget.h"
#include <glib.h>

struct _KrakenPluginManager
{
    GtkBin parent_instance;
};

G_DEFINE_TYPE (KrakenPluginManager, kraken_plugin_manager, GTK_TYPE_BIN);

static void
kraken_plugin_manager_class_init (KrakenPluginManagerClass *klass)
{
}

static void
kraken_plugin_manager_init (KrakenPluginManager *self)
{
    GtkWidget *widget, *grid;

    grid = gtk_grid_new ();

    gtk_widget_set_margin_left (grid, 4);
    gtk_widget_set_margin_right (grid, 4);
    gtk_widget_set_margin_top (grid, 4);
    gtk_widget_set_margin_bottom (grid, 4);
    gtk_grid_set_row_spacing (GTK_GRID (grid), 4);
    gtk_grid_set_column_spacing (GTK_GRID (grid), 4);
    gtk_grid_set_row_homogeneous (GTK_GRID (grid), TRUE);
    gtk_grid_set_column_homogeneous (GTK_GRID (grid), TRUE);

    widget = kraken_action_config_widget_new ();
    gtk_grid_attach (GTK_GRID (grid), widget, 0, 0, 2, 1);

    widget = kraken_extension_config_widget_new ();
    gtk_grid_attach (GTK_GRID (grid), widget, 0, 1, 2, 1);

    gtk_container_add (GTK_CONTAINER (self), grid);

    gtk_widget_show_all (GTK_WIDGET (self));
}

KrakenPluginManager *
kraken_plugin_manager_new (void)
{
  return g_object_new (KRAKEN_TYPE_PLUGIN_MANAGER, NULL);
}

