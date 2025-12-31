/* kraken-script-config-widget.h */

/*  A widget that displays a list of scripts to enable or disable.
 *  This is usually part of a KrakenPluginManagerWidget
 */

#ifndef __KRAKEN_TEMPLATE_CONFIG_WIDGET_H__
#define __KRAKEN_TEMPLATE_CONFIG_WIDGET_H__

#include <glib-object.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>

#include "kraken-config-base-widget.h"

G_BEGIN_DECLS

#define KRAKEN_TYPE_TEMPLATE_CONFIG_WIDGET (kraken_template_config_widget_get_type())

#define KRAKEN_TEMPLATE_CONFIG_WIDGET(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_TEMPLATE_CONFIG_WIDGET, KrakenTemplateConfigWidget))
#define KRAKEN_TEMPLATE_CONFIG_WIDGET_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_TEMPLATE_CONFIG_WIDGET, KrakenTemplateConfigWidgetClass))
#define KRAKEN_IS_TEMPLATE_CONFIG_WIDGET(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_TEMPLATE_CONFIG_WIDGET))
#define KRAKEN_IS_TEMPLATE_CONFIG_WIDGET_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_TEMPLATE_CONFIG_WIDGET))
#define KRAKEN_TEMPLATE_CONFIG_WIDGET_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_TEMPLATE_CONFIG_WIDGET, KrakenTemplateConfigWidgetClass))

typedef struct _KrakenTemplateConfigWidget KrakenTemplateConfigWidget;
typedef struct _KrakenTemplateConfigWidgetClass KrakenTemplateConfigWidgetClass;

struct _KrakenTemplateConfigWidget
{
  KrakenConfigBaseWidget parent;

  GList *templates;

  GList *dir_monitors;
  GtkWidget *remove_button;
  GtkWidget *rename_button;
  GtkWidget *edit_button;
};

struct _KrakenTemplateConfigWidgetClass
{
  KrakenConfigBaseWidgetClass parent_class;
};

GType kraken_template_config_widget_get_type (void);

GtkWidget  *kraken_template_config_widget_new                   (void);

G_END_DECLS

#endif /* __KRAKEN_TEMPLATE_CONFIG_WIDGET_H__ */
