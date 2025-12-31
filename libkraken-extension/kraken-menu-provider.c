/*
 *  kraken-property-page-provider.c - Interface for Kraken extensions 
 *                                      that provide context menu items
 *                                      for files.
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


#include <config.h>
#include "kraken-menu-provider.h"

#include <glib-object.h>

G_DEFINE_INTERFACE (KrakenMenuProvider, kraken_menu_provider, G_TYPE_OBJECT)

/**
 * SECTION:kraken-menu-provider
 * @Title: KrakenMenuProvider
 * @Short_description: Allows additional menu items to be added to context menus.
 *
 * This interface allows custom menu entries to be inserted in the selection and
 * background context menus.  Submenus and separators can also be generated.
 *
 **/

static void
kraken_menu_provider_default_init (KrakenMenuProviderInterface *klass)
{
    static gboolean initialized = FALSE;

    if (!initialized)
    {
        /* This signal should be emited each time the extension modify the list of menu items */
        g_signal_new ("items_updated",
            KRAKEN_TYPE_MENU_PROVIDER,
            G_SIGNAL_RUN_LAST,
            0,
            NULL, NULL,
            g_cclosure_marshal_VOID__VOID,
            G_TYPE_NONE, 0);
        initialized = TRUE;
    }
}

/**
 * kraken_menu_provider_get_file_items:
 * @provider: a #KrakenMenuProvider
 * @window: the parent #GtkWidget window
 * @files: (element-type KrakenFileInfo): a list of #KrakenFileInfo
 *
 * Returns: (element-type KrakenMenuItem) (transfer full): the provided list of #KrakenMenuItem
 */
GList *
kraken_menu_provider_get_file_items (KrakenMenuProvider *provider,
				       GtkWidget *window,
				       GList *files)
{
	g_return_val_if_fail (KRAKEN_IS_MENU_PROVIDER (provider), NULL);

    if (files == NULL) {
        return NULL;
    }

	if (KRAKEN_MENU_PROVIDER_GET_IFACE (provider)->get_file_items) {
		return KRAKEN_MENU_PROVIDER_GET_IFACE (provider)->get_file_items 
			(provider, window, files);
	} else {
		return NULL;
	}
}

/**
 * kraken_menu_provider_get_background_items:
 * @provider: a #KrakenMenuProvider
 * @window: the parent #GtkWidget window
 * @current_folder: the folder for which background items are requested
 *
 * Returns: (element-type KrakenMenuItem) (transfer full): the provided list of #KrakenMenuItem
 */
GList *
kraken_menu_provider_get_background_items (KrakenMenuProvider *provider,
					     GtkWidget *window,
					     KrakenFileInfo *current_folder)
{
	g_return_val_if_fail (KRAKEN_IS_MENU_PROVIDER (provider), NULL);
	g_return_val_if_fail (KRAKEN_IS_FILE_INFO (current_folder), NULL);

	if (KRAKEN_MENU_PROVIDER_GET_IFACE (provider)->get_background_items) {
		return KRAKEN_MENU_PROVIDER_GET_IFACE (provider)->get_background_items 
			(provider, window, current_folder);
	} else {
		return NULL;
	}
}

/* This function emit a signal to inform kraken that its item list has changed */
void
kraken_menu_provider_emit_items_updated_signal (KrakenMenuProvider* provider)
{
	g_return_if_fail (KRAKEN_IS_MENU_PROVIDER (provider));

	g_signal_emit_by_name (provider, "items_updated");
}

