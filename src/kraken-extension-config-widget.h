/* kraken-extension-config-widget.h */

/*  A widget that displays a list of extensions to enable or disable.
 *  This is usually part of a KrakenPluginManagerWidget
 */

#ifndef __KRAKEN_EXTENSION_CONFIG_WIDGET_H__
#define __KRAKEN_EXTENSION_CONFIG_WIDGET_H__

#include <glib-object.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>

#include "kraken-config-base-widget.h"

G_BEGIN_DECLS

#define KRAKEN_TYPE_EXTENSION_CONFIG_WIDGET (kraken_extension_config_widget_get_type())

#define KRAKEN_EXTENSION_CONFIG_WIDGET(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_EXTENSION_CONFIG_WIDGET, KrakenExtensionConfigWidget))
#define KRAKEN_EXTENSION_CONFIG_WIDGET_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_EXTENSION_CONFIG_WIDGET, KrakenExtensionConfigWidgetClass))
#define KRAKEN_IS_EXTENSION_CONFIG_WIDGET(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_EXTENSION_CONFIG_WIDGET))
#define KRAKEN_IS_EXTENSION_CONFIG_WIDGET_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_EXTENSION_CONFIG_WIDGET))
#define KRAKEN_EXTENSION_CONFIG_WIDGET_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_EXTENSION_CONFIG_WIDGET, KrakenExtensionConfigWidgetClass))

typedef struct _KrakenExtensionConfigWidget KrakenExtensionConfigWidget;
typedef struct _KrakenExtensionConfigWidgetClass KrakenExtensionConfigWidgetClass;

struct _KrakenExtensionConfigWidget
{
  KrakenConfigBaseWidget parent;
  GtkWidget *restart_button;

  GList *current_extensions;
  GList *initial_extension_ids;

  gulong bl_handler;
};

struct _KrakenExtensionConfigWidgetClass
{
  KrakenConfigBaseWidgetClass parent_class;
};

GType kraken_extension_config_widget_get_type (void);

GtkWidget  *kraken_extension_config_widget_new                   (void);

G_END_DECLS

#endif /* __KRAKEN_EXTENSION_CONFIG_WIDGET_H__ */
