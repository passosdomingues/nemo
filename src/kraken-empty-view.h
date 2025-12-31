/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* kraken-empty-view.h - interface for empty view of directory.

   Copyright (C) 2006 Free Software Foundation, Inc.
   
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

   Authors: Christian Neumair <chris@gnome-de.org>
*/

#ifndef KRAKEN_EMPTY_VIEW_H
#define KRAKEN_EMPTY_VIEW_H

#include "kraken-view.h"

#define KRAKEN_TYPE_EMPTY_VIEW kraken_empty_view_get_type()
#define KRAKEN_EMPTY_VIEW(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_EMPTY_VIEW, KrakenEmptyView))
#define KRAKEN_EMPTY_VIEW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_EMPTY_VIEW, KrakenEmptyViewClass))
#define KRAKEN_IS_EMPTY_VIEW(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_EMPTY_VIEW))
#define KRAKEN_IS_EMPTY_VIEW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_EMPTY_VIEW))
#define KRAKEN_EMPTY_VIEW_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_EMPTY_VIEW, KrakenEmptyViewClass))

#define KRAKEN_EMPTY_VIEW_ID "OAFIID:Kraken_File_Manager_Empty_View"

typedef struct KrakenEmptyViewDetails KrakenEmptyViewDetails;

typedef struct {
	KrakenView parent_instance;
	KrakenEmptyViewDetails *details;
} KrakenEmptyView;

typedef struct {
	KrakenViewClass parent_class;
} KrakenEmptyViewClass;

GType kraken_empty_view_get_type (void);
void  kraken_empty_view_register (void);

#endif /* KRAKEN_EMPTY_VIEW_H */
