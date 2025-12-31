/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-
 
   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
  
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.
  
   You should have received a copy of the GNU General Public
   License along with this program; if not, write to the
   Free Software Foundation, Inc., 51 Franklin Street - Suite 500,
   Boston, MA 02110-1335, USA.

*/

#ifndef KRAKEN_WIDGET_ACTION_H
#define KRAKEN_WIDGET_ACTION_H

#include <gtk/gtk.h>
#include <glib.h>

#define KRAKEN_TYPE_WIDGET_ACTION kraken_widget_action_get_type()
#define KRAKEN_WIDGET_ACTION(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_WIDGET_ACTION, KrakenWidgetAction))
#define KRAKEN_WIDGET_ACTION_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_WIDGET_ACTION, KrakenWidgetActionClass))
#define KRAKEN_IS_WIDGET_ACTION(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_WIDGET_ACTION))
#define KRAKEN_IS_WIDGET_ACTION_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_WIDGET_ACTION))
#define KRAKEN_WIDGET_ACTION_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_WIDGET_ACTION, KrakenWidgetActionClass))

typedef struct _KrakenWidgetAction KrakenWidgetAction;
typedef struct _KrakenWidgetActionClass KrakenWidgetActionClass;

struct _KrakenWidgetAction {
    GtkAction parent;
    GtkWidget *widget_a;
    GtkWidget *widget_b;
    gboolean a_used;
    gboolean b_used;
};

struct _KrakenWidgetActionClass {
	GtkActionClass parent_class;
};

enum {
  ACTION_SLOT_A = 0,
  ACTION_SLOT_B
};

GType         kraken_widget_action_get_type             (void);
GtkAction    *kraken_widget_action_new                  (const gchar *name,
                                                       GtkWidget *widget_a,
                                                       GtkWidget *widget_b);
void          kraken_widget_action_activate             (KrakenWidgetAction *action);
GtkWidget *   kraken_widget_action_get_widget_a (KrakenWidgetAction *action);
void          kraken_widget_action_set_widget_a (KrakenWidgetAction *action, GtkWidget *widget);
GtkWidget *   kraken_widget_action_get_widget_b (KrakenWidgetAction *action);
void          kraken_widget_action_set_widget_b (KrakenWidgetAction *action, GtkWidget *widget);
#endif /* KRAKEN_WIDGET_ACTION_H */
