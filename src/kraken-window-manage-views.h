/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

/*
 *  Kraken
 *
 *  Copyright (C) 1999, 2000 Red Hat, Inc.
 *  Copyright (C) 1999, 2000, 2001 Eazel, Inc.
 *
 *  Kraken is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  Kraken is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public
 *  License along with this program; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin Street, Suite 500, MA 02110-1335, USA.
 *
 *  Author: Darin Adler <darin@bentspoon.com>
 *
 */

#ifndef KRAKEN_WINDOW_MANAGE_VIEWS_H
#define KRAKEN_WINDOW_MANAGE_VIEWS_H

#include "kraken-window.h"
#include "kraken-window-pane.h"

void kraken_window_manage_views_close_slot (KrakenWindowSlot *slot);


/* KrakenWindowInfo implementation: */
void kraken_window_report_location_change   (KrakenWindow     *window);

#endif /* KRAKEN_WINDOW_MANAGE_VIEWS_H */
