/* kraken-statusbar.h
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street - Suite 500,
 * Boston, MA 02110-1335, USA.
 *
 * 
 */

#ifndef KRAKEN_STATUSBAR_H
#define KRAKEN_STATUSBAR_H

#include <gtk/gtk.h>
#include <gio/gio.h>
#include "kraken-window.h"
#include "kraken-window-slot.h"
#include "kraken-view.h"

typedef struct _KrakenStatusBar      KrakenStatusBar;
typedef struct _KrakenStatusBarClass KrakenStatusBarClass;


#define KRAKEN_TYPE_STATUS_BAR                 (kraken_status_bar_get_type ())
#define KRAKEN_STATUS_BAR(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_STATUS_BAR, KrakenStatusBar))
#define KRAKEN_STATUS_BAR_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_STATUS_BAR, KrakenStatusBarClass))
#define KRAKEN_IS_STATUS_BAR(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_STATUS_BAR))
#define KRAKEN_IS_STATUS_BAR_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_STATUS_BAR))
#define KRAKEN_STATUS_BAR_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_STATUS_BAR, KrakenStatusBarClass))

#define KRAKEN_STATUSBAR_ICON_SIZE_NAME "statusbar-icon"
#define KRAKEN_STATUSBAR_ICON_SIZE 11

struct _KrakenStatusBar
{
    GtkBox parent;
    KrakenWindow *window;
    GtkWidget *real_statusbar;

    GtkWidget *zoom_slider;

    GtkWidget *tree_button;
    GtkWidget *places_button;
    GtkWidget *show_button;
    GtkWidget *hide_button;
    GtkWidget *separator;
};

struct _KrakenStatusBarClass
{
    GtkBoxClass parent_class;
};

GType    kraken_status_bar_get_type (void) G_GNUC_CONST;

GtkWidget *kraken_status_bar_new (KrakenWindow *window);

GtkWidget *kraken_status_bar_get_real_statusbar (KrakenStatusBar *bar);

void       kraken_status_bar_sync_button_states (KrakenStatusBar *bar);

void       kraken_status_bar_sync_zoom_widgets (KrakenStatusBar *bar);

#endif /* KRAKEN_STATUSBAR_H */
