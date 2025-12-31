/* kraken-simple-button.h */

#ifndef __KRAKEN_SIMPLE_BUTTON_H__
#define __KRAKEN_SIMPLE_BUTTON_H__

#include <glib-object.h>
#include <gtk/gtk.h>
#include "kraken-extension-types.h"

G_BEGIN_DECLS

#define KRAKEN_TYPE_SIMPLE_BUTTON kraken_simple_button_get_type()

G_DECLARE_FINAL_TYPE (KrakenSimpleButton, kraken_simple_button, KRAKEN, SIMPLE_BUTTON, GtkButton)

KrakenSimpleButton *kraken_simple_button_new (void);
KrakenSimpleButton *kraken_simple_button_new_from_icon_name (const gchar *icon_name, int icon_size);
KrakenSimpleButton *kraken_simple_button_new_from_stock (const gchar *stock_id, int icon_size);
KrakenSimpleButton *kraken_simple_button_new_from_file (const gchar *path, int icon_size);

G_END_DECLS

#endif /* __KRAKEN_SIMPLE_BUTTON_H__ */
