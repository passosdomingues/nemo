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

#ifndef KRAKEN_ACTION_H
#define KRAKEN_ACTION_H

#include <gtk/gtk.h>
#include <glib.h>
#include "kraken-file.h"

// GtkAction were deprecated before auto-free functionality was added.
G_DEFINE_AUTOPTR_CLEANUP_FUNC (GtkAction, g_object_unref)

#define KRAKEN_TYPE_ACTION kraken_action_get_type()
G_DECLARE_FINAL_TYPE (KrakenAction, kraken_action, KRAKEN, ACTION, GtkAction)

struct _KrakenAction {
    GtkAction parent_instance;

    gchar *uuid; // basename of key_file_path
    gchar *key_file_path;
    gchar *parent_dir;
    gboolean has_accel;
};

struct _KrakenActionClass {
    GtkActionClass parent_class;
};

KrakenAction   *kraken_action_new                  (const gchar *name, const gchar *path);
void          kraken_action_activate             (KrakenAction *action, GList *selection, KrakenFile *parent, GtkWindow *window);

const gchar  *kraken_action_get_orig_label       (KrakenAction *action);
const gchar  *kraken_action_get_orig_tt          (KrakenAction *action);
gchar        *kraken_action_get_label            (KrakenAction *action, GList *selection, KrakenFile *parent, GtkWindow *window);
gchar        *kraken_action_get_tt               (KrakenAction *action, GList *selection, KrakenFile *parent, GtkWindow *window);
void          kraken_action_update_display_state (KrakenAction *action, GList *selection, KrakenFile *parent, gboolean for_places, GtkWindow *window);

// Layout model overrides
void          kraken_action_override_label       (KrakenAction *action, const gchar *label);
void          kraken_action_override_icon        (KrakenAction *action, const gchar *icon_name);
#endif /* KRAKEN_ACTION_H */
