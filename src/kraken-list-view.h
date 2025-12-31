/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* fm-list-view.h - interface for list view of directory.

   Copyright (C) 2000 Eazel, Inc.
   Copyright (C) 2001 Anders Carlsson <andersca@gnu.org>
   
   The Gnome Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The Gnome Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 51 Franklin Street - Suite 500,
   Boston, MA 02110-1335, USA.

   Authors: John Sullivan <sullivan@eazel.com>
            Anders Carlsson <andersca@gnu.org>
*/

#ifndef KRAKEN_LIST_VIEW_H
#define KRAKEN_LIST_VIEW_H

#include "kraken-view.h"

#define KRAKEN_TYPE_LIST_VIEW kraken_list_view_get_type()
#define KRAKEN_LIST_VIEW(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_LIST_VIEW, KrakenListView))
#define KRAKEN_LIST_VIEW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_LIST_VIEW, KrakenListViewClass))
#define KRAKEN_IS_LIST_VIEW(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_LIST_VIEW))
#define KRAKEN_IS_LIST_VIEW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_LIST_VIEW))
#define KRAKEN_LIST_VIEW_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_LIST_VIEW, KrakenListViewClass))

#define KRAKEN_LIST_VIEW_ID "OAFIID:Kraken_File_Manager_List_View"

typedef struct KrakenListViewDetails KrakenListViewDetails;

typedef struct {
	KrakenView parent_instance;
	KrakenListViewDetails *details;
} KrakenListView;

typedef struct {
	KrakenViewClass parent_class;
} KrakenListViewClass;

GType kraken_list_view_get_type (void);
void  kraken_list_view_register (void);
GtkTreeView* kraken_list_view_get_tree_view (KrakenListView *list_view);

#endif /* KRAKEN_LIST_VIEW_H */
