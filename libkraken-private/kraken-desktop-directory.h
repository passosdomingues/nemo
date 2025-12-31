/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   kraken-desktop-directory.h: Subclass of KrakenDirectory to implement
   a virtual directory consisting of the desktop directory and the desktop
   icons
 
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

#ifndef KRAKEN_DESKTOP_DIRECTORY_H
#define KRAKEN_DESKTOP_DIRECTORY_H

#include <libkraken-private/kraken-directory.h>

#define KRAKEN_TYPE_DESKTOP_DIRECTORY kraken_desktop_directory_get_type()
#define KRAKEN_DESKTOP_DIRECTORY(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_DESKTOP_DIRECTORY, KrakenDesktopDirectory))
#define KRAKEN_DESKTOP_DIRECTORY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_DESKTOP_DIRECTORY, KrakenDesktopDirectoryClass))
#define KRAKEN_IS_DESKTOP_DIRECTORY(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_DESKTOP_DIRECTORY))
#define KRAKEN_IS_DESKTOP_DIRECTORY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_DESKTOP_DIRECTORY))
#define KRAKEN_DESKTOP_DIRECTORY_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_DESKTOP_DIRECTORY, KrakenDesktopDirectoryClass))

typedef struct KrakenDesktopDirectoryDetails KrakenDesktopDirectoryDetails;

typedef struct {
	KrakenDirectory parent_slot;
	KrakenDesktopDirectoryDetails *details;
    gint display_number;
} KrakenDesktopDirectory;

typedef struct {
	KrakenDirectoryClass parent_slot;

} KrakenDesktopDirectoryClass;

GType   kraken_desktop_directory_get_type             (void);
KrakenDirectory * kraken_desktop_directory_get_real_directory   (KrakenDesktopDirectory *desktop_directory);

#endif /* KRAKEN_DESKTOP_DIRECTORY_H */
