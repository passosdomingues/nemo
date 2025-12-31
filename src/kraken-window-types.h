/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */

/*
 *  kraken-window-types: typedefs for window-related types.
 *
 *  Copyright (C) 1999, 2000, 2010 Red Hat, Inc.
 *  Copyright (C) 1999, 2000, 2001 Eazel, Inc.
 *
 *  Kraken is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  Kraken is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Suite 500, MA 02110-1335, USA.
 *
 *  Authors: Elliot Lee <sopwith@redhat.com>
 *           Darin Adler <darin@bentspoon.com>
 *
 */

#ifndef __KRAKEN_WINDOW_TYPES_H__
#define __KRAKEN_WINDOW_TYPES_H__

typedef struct _KrakenWindowPane KrakenWindowPane;
typedef struct _KrakenWindowPaneClass KrakenWindowPaneClass;

typedef struct KrakenWindow KrakenWindow;

typedef struct KrakenWindowSlot KrakenWindowSlot;
typedef struct KrakenWindowSlotClass KrakenWindowSlotClass;

typedef void (* KrakenWindowGoToCallback) (KrakenWindow *window,
                                             GError *error,
                                             gpointer user_data);

typedef enum {
        KRAKEN_WINDOW_OPEN_FLAG_CLOSE_BEHIND = 1<<0,
        KRAKEN_WINDOW_OPEN_FLAG_NEW_WINDOW = 1<<1,
        KRAKEN_WINDOW_OPEN_FLAG_NEW_TAB = 1<<2,
        KRAKEN_WINDOW_OPEN_FLAG_SEARCH = 1<<3,
        KRAKEN_WINDOW_OPEN_FLAG_MOUNT = 1<<4
} KrakenWindowOpenFlags;

#endif /* __KRAKEN_WINDOW_TYPES_H__ */
