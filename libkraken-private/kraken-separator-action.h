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

#ifndef KRAKEN_SEPARATOR_ACTION_H
#define KRAKEN_SEPARATOR_ACTION_H

#include <gtk/gtk.h>

#define KRAKEN_TYPE_SEPARATOR_ACTION kraken_separator_action_get_type()
#define KRAKEN_SEPARATOR_ACTION(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_SEPARATOR_ACTION, KrakenSeparatorAction))
#define KRAKEN_SEPARATOR_ACTION_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_SEPARATOR_ACTION, KrakenSeparatorActionClass))
#define KRAKEN_IS_SEPARATOR_ACTION(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_SEPARATOR_ACTION))
#define KRAKEN_IS_SEPARATOR_ACTION_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_SEPARATOR_ACTION))
#define KRAKEN_SEPARATOR_ACTION_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_SEPARATOR_ACTION, KrakenSeparatorActionClass))

typedef struct _KrakenSeparatorAction KrakenSeparatorAction;
typedef struct _KrakenSeparatorActionClass KrakenSeparatorActionClass;

struct _KrakenSeparatorAction {
    GtkAction parent;
};

struct _KrakenSeparatorActionClass {
	GtkActionClass parent_class;
};

GType         kraken_separator_action_get_type             (void);
GtkAction    *kraken_separator_action_new                  (const gchar *name);

#endif /* KRAKEN_SEPARATOR_ACTION_H */
