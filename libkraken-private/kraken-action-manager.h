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

#ifndef KRAKEN_ACTION_MANAGER_H
#define KRAKEN_ACTION_MANAGER_H

#include <glib.h>
#include <json-glib/json-glib.h>

#include "kraken-action.h"

#define KRAKEN_TYPE_ACTION_MANAGER kraken_action_manager_get_type()

G_DECLARE_FINAL_TYPE (KrakenActionManager, kraken_action_manager, KRAKEN, ACTION_MANAGER, GObject)

typedef void (* KrakenActionManagerIterFunc) (KrakenActionManager    *manager,
                                            GtkAction            *action,
                                            GtkUIManagerItemType  type,
                                            const gchar          *path,
                                            const gchar          *accelerator,
                                            gpointer              user_data);

KrakenActionManager   * kraken_action_manager_new                       (void);
GList               * kraken_action_manager_list_actions              (KrakenActionManager *action_manager);
JsonReader          * kraken_action_manager_get_layout_reader         (KrakenActionManager *action_manager);
gchar               * kraken_action_manager_get_system_directory_path (const gchar *data_dir);
gchar               * kraken_action_manager_get_user_directory_path   (void);
KrakenAction          * kraken_action_manager_get_action                (KrakenActionManager *action_manager,
                                                                     const gchar       *uuid);
void                  kraken_action_manager_iterate_actions           (KrakenActionManager                *action_manager,
                                                                     KrakenActionManagerIterFunc         callback,
                                                                     gpointer                          user_data);
void                  kraken_action_manager_update_action_states      (KrakenActionManager *action_manager,
                                                                     GtkActionGroup    *action_group,
                                                                     GList             *selection,
                                                                     KrakenFile          *parent,
                                                                     gboolean           for_places,
                                                                     gboolean           for_accelerators,
                                                                     GtkWindow         *window);

void                  kraken_action_manager_add_action_ui             (KrakenActionManager   *manager,
                                                                     GtkUIManager        *ui_manager,
                                                                     GtkAction           *action,
                                                                     const gchar         *action_path,
                                                                     const gchar         *accelerator,
                                                                     GtkActionGroup      *action_group,
                                                                     guint                merge_id,
                                                                     const gchar        **placeholder_paths,
                                                                     GtkUIManagerItemType type,
                                                                     GCallback            activate_callback,
                                                                     gpointer             user_data);

#endif /* KRAKEN_ACTION_MANAGER_H */

