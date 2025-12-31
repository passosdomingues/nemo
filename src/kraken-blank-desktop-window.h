/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * Kraken
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
 */

/* kraken-blank-desktop-window.h
 */

#ifndef KRAKEN_BLANK_DESKTOP_WINDOW_H
#define KRAKEN_BLANK_DESKTOP_WINDOW_H

#include <gtk/gtk.h>

#include <libkraken-private/kraken-action-manager.h>

#define KRAKEN_TYPE_BLANK_DESKTOP_WINDOW kraken_blank_desktop_window_get_type()
#define KRAKEN_BLANK_DESKTOP_WINDOW(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_BLANK_DESKTOP_WINDOW, KrakenBlankDesktopWindow))
#define KRAKEN_BLANK_DESKTOP_WINDOW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_BLANK_DESKTOP_WINDOW, KrakenBlankDesktopWindowClass))
#define KRAKEN_IS_BLANK_DESKTOP_WINDOW(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_BLANK_DESKTOP_WINDOW))
#define KRAKEN_IS_BLANK_DESKTOP_WINDOW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_BLANK_DESKTOP_WINDOW))
#define KRAKEN_BLANK_DESKTOP_WINDOW_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_BLANK_DESKTOP_WINDOW, KrakenBlankDesktopWindowClass))

typedef struct KrakenBlankDesktopWindowDetails KrakenBlankDesktopWindowDetails;

typedef struct {
	GtkWindow parent_spot;
	KrakenBlankDesktopWindowDetails *details;
} KrakenBlankDesktopWindow;

typedef struct {
	GtkWindowClass parent_spot;

    void   (* plugin_manager)  (KrakenBlankDesktopWindow *window);
} KrakenBlankDesktopWindowClass;

GType                   kraken_blank_desktop_window_get_type            (void);
KrakenBlankDesktopWindow *kraken_blank_desktop_window_new                 (gint monitor);
KrakenActionManager      *kraken_desktop_manager_get_action_manager       (void);
void                    kraken_blank_desktop_window_update_geometry     (KrakenBlankDesktopWindow *window);

#endif /* KRAKEN_BLANK_DESKTOP_WINDOW_H */
