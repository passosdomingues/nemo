/*
 *  kraken-menu-provider.h - Interface for Kraken extensions that 
 *                             provide context menu items.
 *
 *  Copyright (C) 2003 Novell, Inc.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin Street, Suite 500, MA 02110-1335, USA.
 * 
 *  Author:  Dave Camp <dave@ximian.com>
 *
 */

/* This interface is implemented by Kraken extensions that want to
 * add context menu entries to files.  Extensions are called when
 * Kraken constructs the context menu for a file.  They are passed a
 * list of KrakenFileInfo objects which holds the current selection */

#ifndef KRAKEN_MENU_PROVIDER_H
#define KRAKEN_MENU_PROVIDER_H

#include <glib-object.h>
#include <gtk/gtk.h>
#include "kraken-extension-types.h"
#include "kraken-file-info.h"
#include "kraken-menu.h"

G_BEGIN_DECLS

#define KRAKEN_TYPE_MENU_PROVIDER           (kraken_menu_provider_get_type ())

G_DECLARE_INTERFACE (KrakenMenuProvider, kraken_menu_provider,
                     KRAKEN, MENU_PROVIDER,
                     GObject)

typedef KrakenMenuProviderInterface KrakenMenuProviderIface;

struct _KrakenMenuProviderInterface {
	GTypeInterface g_iface;

	GList *(*get_file_items)       (KrakenMenuProvider *provider,
					GtkWidget            *window,
					GList                *files);
	GList *(*get_background_items) (KrakenMenuProvider *provider,
					GtkWidget            *window,
					KrakenFileInfo     *current_folder);
};

/* Interface Functions */
GList                  *kraken_menu_provider_get_file_items       (KrakenMenuProvider *provider,
								     GtkWidget            *window,
								     GList                *files);
GList                  *kraken_menu_provider_get_background_items (KrakenMenuProvider *provider,
								     GtkWidget            *window,
								     KrakenFileInfo     *current_folder);

/* This function emit a signal to inform kraken that its item list has changed. */
void                    kraken_menu_provider_emit_items_updated_signal (KrakenMenuProvider *provider);

G_END_DECLS

#endif
