/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   kraken-desktop-link.h: Class that handles the links on the desktop
    
   Copyright (C) 2003 Red Hat, Inc.
  
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
  
   Author: Alexander Larsson <alexl@redhat.com>
*/

#ifndef KRAKEN_DESKTOP_LINK_H
#define KRAKEN_DESKTOP_LINK_H

#include <libkraken-private/kraken-file.h>
#include <gio/gio.h>

#define KRAKEN_TYPE_DESKTOP_LINK kraken_desktop_link_get_type()
#define KRAKEN_DESKTOP_LINK(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_DESKTOP_LINK, KrakenDesktopLink))
#define KRAKEN_DESKTOP_LINK_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_DESKTOP_LINK, KrakenDesktopLinkClass))
#define KRAKEN_IS_DESKTOP_LINK(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_DESKTOP_LINK))
#define KRAKEN_IS_DESKTOP_LINK_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_DESKTOP_LINK))
#define KRAKEN_DESKTOP_LINK_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_DESKTOP_LINK, KrakenDesktopLinkClass))

typedef struct KrakenDesktopLinkDetails KrakenDesktopLinkDetails;

typedef struct {
	GObject parent_slot;
	KrakenDesktopLinkDetails *details;
} KrakenDesktopLink;

typedef struct {
	GObjectClass parent_slot;
} KrakenDesktopLinkClass;

typedef enum {
	KRAKEN_DESKTOP_LINK_HOME,
	KRAKEN_DESKTOP_LINK_COMPUTER,
	KRAKEN_DESKTOP_LINK_TRASH,
	KRAKEN_DESKTOP_LINK_MOUNT,
	KRAKEN_DESKTOP_LINK_NETWORK
} KrakenDesktopLinkType;

GType   kraken_desktop_link_get_type (void);

KrakenDesktopLink *   kraken_desktop_link_new                     (KrakenDesktopLinkType  type);
KrakenDesktopLink *   kraken_desktop_link_new_from_mount          (GMount                 *mount);
KrakenDesktopLinkType kraken_desktop_link_get_link_type           (KrakenDesktopLink     *link);
KrakenFile *          kraken_desktop_link_get_file                (KrakenDesktopLink *link);
char *                  kraken_desktop_link_get_file_name           (KrakenDesktopLink     *link);
char *                  kraken_desktop_link_get_display_name        (KrakenDesktopLink     *link);
GIcon *                 kraken_desktop_link_get_icon                (KrakenDesktopLink     *link);
GFile *                 kraken_desktop_link_get_activation_location (KrakenDesktopLink     *link);
char *                  kraken_desktop_link_get_activation_uri      (KrakenDesktopLink     *link);
gboolean                kraken_desktop_link_get_date                (KrakenDesktopLink     *link,
								       KrakenDateType         date_type,
								       time_t                  *date);
GMount *                kraken_desktop_link_get_mount               (KrakenDesktopLink     *link);
gboolean                kraken_desktop_link_can_rename              (KrakenDesktopLink     *link);
gboolean                kraken_desktop_link_rename                  (KrakenDesktopLink     *link,
								       const char              *name);


#endif /* KRAKEN_DESKTOP_LINK_H */
