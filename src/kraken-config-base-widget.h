/* kraken-config-base-widget.h */

/*  A base widget class for extension/action/script config widgets.
 *  This is usually part of a KrakenPluginManagerWidget
 */

#ifndef __KRAKEN_CONFIG_BASE_WIDGET_H__
#define __KRAKEN_CONFIG_BASE_WIDGET_H__

#include <glib-object.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>

#include "kraken-window-private.h"

G_BEGIN_DECLS

#define KRAKEN_TYPE_CONFIG_BASE_WIDGET (kraken_config_base_widget_get_type())

#define KRAKEN_CONFIG_BASE_WIDGET(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_CONFIG_BASE_WIDGET, KrakenConfigBaseWidget))
#define KRAKEN_CONFIG_BASE_WIDGET_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_CONFIG_BASE_WIDGET, KrakenConfigBaseWidgetClass))
#define KRAKEN_IS_CONFIG_BASE_WIDGET(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_CONFIG_BASE_WIDGET))
#define KRAKEN_IS_CONFIG_BASE_WIDGET_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_CONFIG_BASE_WIDGET))
#define KRAKEN_CONFIG_BASE_WIDGET_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_CONFIG_BASE_WIDGET, KrakenConfigBaseWidgetClass))

typedef struct _KrakenConfigBaseWidget KrakenConfigBaseWidget;
typedef struct _KrakenConfigBaseWidgetClass KrakenConfigBaseWidgetClass;

struct _KrakenConfigBaseWidget
{
  GtkBin parent;

  GtkWidget *label;
  GtkWidget *listbox;
  GtkWidget *lbuttonbox;
  GtkWidget *rbuttonbox;
  GtkWidget *enable_button;
  GtkWidget *disable_button;
};

struct _KrakenConfigBaseWidgetClass
{
  GtkBinClass parent_class;
};

GType kraken_config_base_widget_get_type (void);

GtkWidget *kraken_config_base_widget_get_label          (KrakenConfigBaseWidget *widget);
GtkWidget *kraken_config_base_widget_get_listbox        (KrakenConfigBaseWidget *widget);
GtkWidget *kraken_config_base_widget_get_enable_button  (KrakenConfigBaseWidget *widget);
GtkWidget *kraken_config_base_widget_get_disable_button (KrakenConfigBaseWidget *widget);

void       kraken_config_base_widget_set_default_buttons_sensitive (KrakenConfigBaseWidget *widget, gboolean sensitive);

void       kraken_config_base_widget_clear_list         (KrakenConfigBaseWidget *widget);
KrakenWindow *kraken_config_base_widget_get_view_window   (KrakenConfigBaseWidget *widget, KrakenWindow *view_window);

G_END_DECLS

#endif /* __KRAKEN_CONFIG_BASE_WIDGET_H__ */
