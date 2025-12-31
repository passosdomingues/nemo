/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* fm-icon-view.h - interface for icon view of directory.

   Copyright (C) 2000 Eazel, Inc.

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

   Authors: Mike Engber <engber@eazel.com>
*/

#ifndef KRAKEN_DESKTOP_ICON_VIEW_H
#define KRAKEN_DESKTOP_ICON_VIEW_H

#include "kraken-icon-view.h"

#define KRAKEN_TYPE_DESKTOP_ICON_VIEW kraken_desktop_icon_view_get_type()
#define KRAKEN_DESKTOP_ICON_VIEW(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_DESKTOP_ICON_VIEW, KrakenDesktopIconView))
#define KRAKEN_DESKTOP_ICON_VIEW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_DESKTOP_ICON_VIEW, KrakenDesktopIconViewClass))
#define KRAKEN_IS_DESKTOP_ICON_VIEW(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_DESKTOP_ICON_VIEW))
#define KRAKEN_IS_DESKTOP_ICON_VIEW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_DESKTOP_ICON_VIEW))
#define KRAKEN_DESKTOP_ICON_VIEW_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_DESKTOP_ICON_VIEW, KrakenDesktopIconViewClass))

#define KRAKEN_DESKTOP_ICON_VIEW_ID "OAFIID:Kraken_File_Manager_Desktop_Icon_View"

typedef struct KrakenDesktopIconViewDetails KrakenDesktopIconViewDetails;
typedef struct {
	KrakenIconView parent;
	KrakenDesktopIconViewDetails *details;
} KrakenDesktopIconView;

typedef struct {
	KrakenIconViewClass parent_class;
} KrakenDesktopIconViewClass;

/* GObject support */
GType   kraken_desktop_icon_view_get_type (void);
void kraken_desktop_icon_view_register (void);

#endif /* KRAKEN_DESKTOP_ICON_VIEW_H */
