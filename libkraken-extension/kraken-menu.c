/*
 *  kraken-menu.h - Menus exported by KrakenMenuProvider objects.
 *
 *  Copyright (C) 2005 Raffaele Sandrini
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
 *  Author:  Raffaele Sandrini <rasa@gmx.ch>
 *
 */

#include <config.h>
#include "kraken-menu.h"
#include "kraken-extension-i18n.h"

#include <glib.h>

typedef struct {
	GList *item_list;
} KrakenMenuPrivate;

struct _KrakenMenu
{
    GObject parent_class;

    KrakenMenuPrivate *priv;
};

G_DEFINE_TYPE_WITH_PRIVATE (KrakenMenu, kraken_menu, G_TYPE_OBJECT)

/**
 * SECTION:kraken-menu
 * @Title: KrakenMenu
 * @Short_description: A menu added to Kraken's context menus by an extension
 *
 * Menu items and submenus can be added to Kraken's selected item and background
 * context menus by a #KrakenMenuProvider.  Separators and embedded widgets are also
 * possible (see #KrakenSimpleButton.)
 **/

void
kraken_menu_append_item (KrakenMenu *menu, KrakenMenuItem *item)
{
	g_return_if_fail (menu != NULL);
	g_return_if_fail (item != NULL);
	
	menu->priv->item_list = g_list_append (menu->priv->item_list, g_object_ref (item));
}

/**
 * kraken_menu_get_items:
 * @menu: a #KrakenMenu
 *
 * Returns: (element-type KrakenMenuItem) (transfer full): the provided #KrakenMenuItem list
 */
GList *
kraken_menu_get_items (KrakenMenu *menu)
{
	GList *item_list;

	g_return_val_if_fail (menu != NULL, NULL);
	
	item_list = g_list_copy (menu->priv->item_list);
	g_list_foreach (item_list, (GFunc)g_object_ref, NULL);
	
	return item_list;
}

/**
 * kraken_menu_item_list_free:
 * @item_list: (element-type KrakenMenuItem): a list of #KrakenMenuItem
 *
 */
void
kraken_menu_item_list_free (GList *item_list)
{
	g_return_if_fail (item_list != NULL);
	
	g_list_foreach (item_list, (GFunc)g_object_unref, NULL);
	g_list_free (item_list);
}

/* Type initialization */

static void
kraken_menu_finalize (GObject *object)
{
	KrakenMenu *menu = KRAKEN_MENU (object);

	if (menu->priv->item_list) {
        kraken_menu_item_list_free (menu->priv->item_list);
	}

	G_OBJECT_CLASS (kraken_menu_parent_class)->finalize (object);
}

static void
kraken_menu_init (KrakenMenu *menu)
{
	menu->priv = G_TYPE_INSTANCE_GET_PRIVATE (menu, KRAKEN_TYPE_MENU, KrakenMenuPrivate);

    menu->priv->item_list = NULL;
}

static void
kraken_menu_class_init (KrakenMenuClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->finalize = kraken_menu_finalize;
}

/* public constructors */

KrakenMenu *
kraken_menu_new (void)
{
	KrakenMenu *obj;
	
	obj = KRAKEN_MENU (g_object_new (KRAKEN_TYPE_MENU, NULL));
	
	return obj;
}
