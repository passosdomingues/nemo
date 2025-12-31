/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * Kraken
 *
 * Copyright (C) 2005 Red Hat, Inc.
 *
 * Kraken is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * Kraken is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Suite 500, MA 02110-1335, USA.
 *
 * Author:  Alexander Larsson <alexl@redhat.com>
 */

#ifndef KRAKEN_WINDOW_BOOKMARKS_H
#define KRAKEN_WINDOW_BOOKMARKS_H

#include <libkraken-private/kraken-bookmark.h>
#include <kraken-window.h>
#include "kraken-bookmark-list.h"

void                  kraken_bookmarks_exiting                        (void);
void                  kraken_window_add_bookmark_for_current_location (KrakenWindow *window);
void                  kraken_window_edit_bookmarks                    (KrakenWindow *window);
void                  kraken_window_initialize_bookmarks_menu         (KrakenWindow *window);

#endif
