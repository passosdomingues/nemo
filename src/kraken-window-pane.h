/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   kraken-window-pane.h: Kraken window pane

   Copyright (C) 2008 Free Software Foundation, Inc.

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

   Author: Holger Berndt <berndth@gmx.de>
*/

#ifndef KRAKEN_WINDOW_PANE_H
#define KRAKEN_WINDOW_PANE_H

#include <glib-object.h>

#include "kraken-window.h"

#include <libkraken-private/kraken-icon-info.h>

#define KRAKEN_TYPE_WINDOW_PANE	 (kraken_window_pane_get_type())
#define KRAKEN_WINDOW_PANE_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), KRAKEN_TYPE_WINDOW_PANE, KrakenWindowPaneClass))
#define KRAKEN_WINDOW_PANE(obj)	 (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_WINDOW_PANE, KrakenWindowPane))
#define KRAKEN_IS_WINDOW_PANE(obj)      (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_WINDOW_PANE))
#define KRAKEN_IS_WINDOW_PANE_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), KRAKEN_TYPE_WINDOW_PANE))
#define KRAKEN_WINDOW_PANE_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), KRAKEN_TYPE_WINDOW_PANE, KrakenWindowPaneClass))

struct _KrakenWindowPaneClass {
	GtkBoxClass parent_class;
};

/* A KrakenWindowPane is a layer between a slot and a window.
 * Each slot is contained in one pane, and each pane can contain
 * one or more slots. It also supports the notion of an "active slot".
 * On the other hand, each pane is contained in a window, while each
 * window can contain one or multiple panes. Likewise, the window has
 * the notion of an "active pane".
 *
 * A navigation window may have one or more panes.
 */
struct _KrakenWindowPane {
	GtkBox parent;

	/* hosting window */
	KrakenWindow *window;

	/* available slots, and active slot.
	 * Both of them may never be NULL. */
	GList *slots;
	KrakenWindowSlot *active_slot;

	/* location bar */
	GtkWidget *location_bar;
	GtkWidget *path_bar;
	GtkWidget *search_bar;
	GtkWidget *tool_bar;

	gboolean temporary_navigation_bar;
	gboolean temporary_search_bar;

	gboolean show_location_entry;

	/* notebook */
	GtkWidget *notebook;

	GtkActionGroup *action_group;
	GtkActionGroup *toolbar_action_group;

	GtkWidget *last_focus_widget;
};

GType kraken_window_pane_get_type (void);

KrakenWindowPane *kraken_window_pane_new (KrakenWindow *window);

KrakenWindowSlot *kraken_window_pane_open_slot (KrakenWindowPane *pane,
					    KrakenWindowOpenSlotFlags flags);
/* This removes the slot from the given pane but does not close the pane and/or
 * window as well if there are no more slots left afterwards. This
 * functionality is provided by `kraken_window_pane_close_slot' below.
 */
void kraken_window_pane_remove_slot_unsafe (KrakenWindowPane *pane,
					  KrakenWindowSlot *slot);

void kraken_window_pane_sync_location_widgets (KrakenWindowPane *pane);
void kraken_window_pane_sync_search_widgets (KrakenWindowPane *pane);
void kraken_window_pane_set_active (KrakenWindowPane *pane, gboolean is_active);
void kraken_window_pane_close_slot (KrakenWindowPane *pane, KrakenWindowSlot *slot);
GtkActionGroup * kraken_window_pane_get_toolbar_action_group (KrakenWindowPane   *pane);
void kraken_window_pane_grab_focus (KrakenWindowPane *pane);
void kraken_window_pane_sync_up_actions (KrakenWindowPane *pane);
/* bars */
void     kraken_window_pane_ensure_location_bar (KrakenWindowPane *pane);

#endif /* KRAKEN_WINDOW_PANE_H */
