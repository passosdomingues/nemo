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

#ifndef KRAKEN_DESKTOP_UTILS_H
#define KRAKEN_DESKTOP_UTILS_H

#include <gdk/gdk.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

void kraken_desktop_utils_get_monitor_work_rect (gint num, GdkRectangle *rect);
void kraken_desktop_utils_get_monitor_geometry (gint num, GdkRectangle *rect);
gint kraken_desktop_utils_get_primary_monitor (void);
gint kraken_desktop_utils_get_monitor_for_widget (GtkWidget *widget);
gint kraken_desktop_utils_get_num_monitors (void);
gboolean kraken_desktop_utils_get_monitor_cloned (gint monitor, gint x_primary);
gint kraken_desktop_utils_get_scale_factor (void);

G_END_DECLS

#endif
