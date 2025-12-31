/* kraken-action-config-widget.h */

/*  A widget that displays a list of actions to enable or disable.
 *  This is usually part of a KrakenPluginManagerWidget
 */

#ifndef __KRAKEN_ACTION_CONFIG_WIDGET_H__
#define __KRAKEN_ACTION_CONFIG_WIDGET_H__

#include <glib-object.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>

#include "kraken-config-base-widget.h"

G_BEGIN_DECLS

#define KRAKEN_TYPE_ACTION_CONFIG_WIDGET (kraken_action_config_widget_get_type())

#define KRAKEN_ACTION_CONFIG_WIDGET(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_ACTION_CONFIG_WIDGET, KrakenActionConfigWidget))
#define KRAKEN_ACTION_CONFIG_WIDGET_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_ACTION_CONFIG_WIDGET, KrakenActionConfigWidgetClass))
#define KRAKEN_IS_ACTION_CONFIG_WIDGET(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_ACTION_CONFIG_WIDGET))
#define KRAKEN_IS_ACTION_CONFIG_WIDGET_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_ACTION_CONFIG_WIDGET))
#define KRAKEN_ACTION_CONFIG_WIDGET_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_ACTION_CONFIG_WIDGET, KrakenActionConfigWidgetClass))

typedef struct _KrakenActionConfigWidget KrakenActionConfigWidget;
typedef struct _KrakenActionConfigWidgetClass KrakenActionConfigWidgetClass;

struct _KrakenActionConfigWidget
{
  KrakenConfigBaseWidget parent;

  GList *actions;

  GList *dir_monitors;
  gulong bl_handler;
};

struct _KrakenActionConfigWidgetClass
{
  KrakenConfigBaseWidgetClass parent_class;
};

GType kraken_action_config_widget_get_type (void);

GtkWidget  *kraken_action_config_widget_new                   (void);

G_END_DECLS

#endif /* __KRAKEN_ACTION_CONFIG_WIDGET_H__ */
