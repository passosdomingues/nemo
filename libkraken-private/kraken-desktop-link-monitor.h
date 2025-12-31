/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   kraken-desktop-link-monitor.h: singleton that manages the desktop links
    
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

#ifndef KRAKEN_DESKTOP_LINK_MONITOR_H
#define KRAKEN_DESKTOP_LINK_MONITOR_H

#include <gtk/gtk.h>
#include <libkraken-private/kraken-desktop-link.h>

#define KRAKEN_TYPE_DESKTOP_LINK_MONITOR kraken_desktop_link_monitor_get_type()
#define KRAKEN_DESKTOP_LINK_MONITOR(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_DESKTOP_LINK_MONITOR, KrakenDesktopLinkMonitor))
#define KRAKEN_DESKTOP_LINK_MONITOR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_DESKTOP_LINK_MONITOR, KrakenDesktopLinkMonitorClass))
#define KRAKEN_IS_DESKTOP_LINK_MONITOR(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_DESKTOP_LINK_MONITOR))
#define KRAKEN_IS_DESKTOP_LINK_MONITOR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_DESKTOP_LINK_MONITOR))
#define KRAKEN_DESKTOP_LINK_MONITOR_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_DESKTOP_LINK_MONITOR, KrakenDesktopLinkMonitorClass))

typedef struct KrakenDesktopLinkMonitorDetails KrakenDesktopLinkMonitorDetails;

typedef struct {
	GObject parent_slot;
	KrakenDesktopLinkMonitorDetails *details;
} KrakenDesktopLinkMonitor;

typedef struct {
	GObjectClass parent_slot;
} KrakenDesktopLinkMonitorClass;

GType   kraken_desktop_link_monitor_get_type (void);

KrakenDesktopLinkMonitor *   kraken_desktop_link_monitor_get (void);
void kraken_desktop_link_monitor_delete_link (KrakenDesktopLinkMonitor *monitor,
						KrakenDesktopLink *link,
						GtkWidget *parent_view);

/* Used by kraken-desktop-link.c */
char * kraken_desktop_link_monitor_make_filename_unique (KrakenDesktopLinkMonitor *monitor,
							   const char *filename);

#endif /* KRAKEN_DESKTOP_LINK_MONITOR_H */
