/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * Kraken
 *
 * Copyright (C) 2000 Eazel, Inc.
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
 * Authors: Darin Adler <darin@bentspoon.com>
 */

/* kraken-desktop-window.h
 */

#ifndef KRAKEN_DESKTOP_WINDOW_H
#define KRAKEN_DESKTOP_WINDOW_H

#include "kraken-window.h"

#define KRAKEN_TYPE_DESKTOP_WINDOW kraken_desktop_window_get_type()
#define KRAKEN_DESKTOP_WINDOW(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_DESKTOP_WINDOW, KrakenDesktopWindow))
#define KRAKEN_DESKTOP_WINDOW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_DESKTOP_WINDOW, KrakenDesktopWindowClass))
#define KRAKEN_IS_DESKTOP_WINDOW(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_DESKTOP_WINDOW))
#define KRAKEN_IS_DESKTOP_WINDOW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_DESKTOP_WINDOW))
#define KRAKEN_DESKTOP_WINDOW_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_DESKTOP_WINDOW, KrakenDesktopWindowClass))

typedef struct KrakenDesktopWindowDetails KrakenDesktopWindowDetails;

typedef struct {
	KrakenWindow parent_spot;
	KrakenDesktopWindowDetails *details;
} KrakenDesktopWindow;

typedef struct {
	KrakenWindowClass parent_spot;
} KrakenDesktopWindowClass;

GType                  kraken_desktop_window_get_type            (void);
KrakenDesktopWindow     *kraken_desktop_window_new                 (gint monitor);
gboolean               kraken_desktop_window_loaded              (KrakenDesktopWindow *window);
gint                   kraken_desktop_window_get_monitor         (KrakenDesktopWindow *window);
void                   kraken_desktop_window_update_geometry     (KrakenDesktopWindow *window);
gboolean               kraken_desktop_window_get_grid_adjusts    (KrakenDesktopWindow *window,
                                                                gint              *h_adjust,
                                                                gint              *v_adjust);
gboolean               kraken_desktop_window_set_grid_adjusts    (KrakenDesktopWindow *window,
                                                                gint               h_adjust,
                                                                gint               v_adjust);
GtkActionGroup *       kraken_desktop_window_get_action_group (KrakenDesktopWindow *window);
#endif /* KRAKEN_DESKTOP_WINDOW_H */
