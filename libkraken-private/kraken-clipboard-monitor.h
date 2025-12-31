/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   kraken-clipboard-monitor.h: lets you notice clipboard changes.
    
   Copyright (C) 2004 Red Hat, Inc.
  
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

#ifndef KRAKEN_CLIPBOARD_MONITOR_H
#define KRAKEN_CLIPBOARD_MONITOR_H

#include <gtk/gtk.h>

#define KRAKEN_TYPE_CLIPBOARD_MONITOR kraken_clipboard_monitor_get_type()
#define KRAKEN_CLIPBOARD_MONITOR(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_CLIPBOARD_MONITOR, KrakenClipboardMonitor))
#define KRAKEN_CLIPBOARD_MONITOR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_CLIPBOARD_MONITOR, KrakenClipboardMonitorClass))
#define KRAKEN_IS_CLIPBOARD_MONITOR(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_CLIPBOARD_MONITOR))
#define KRAKEN_IS_CLIPBOARD_MONITOR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_CLIPBOARD_MONITOR))
#define KRAKEN_CLIPBOARD_MONITOR_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_CLIPBOARD_MONITOR, KrakenClipboardMonitorClass))

typedef struct KrakenClipboardMonitorDetails KrakenClipboardMonitorDetails;
typedef struct KrakenClipboardInfo KrakenClipboardInfo;

typedef struct {
	GObject parent_slot;

	KrakenClipboardMonitorDetails *details;
} KrakenClipboardMonitor;

typedef struct {
	GObjectClass parent_slot;
  
	void (* clipboard_changed) (KrakenClipboardMonitor *monitor);
	void (* clipboard_info) (KrakenClipboardMonitor *monitor,
	                         KrakenClipboardInfo *info);
} KrakenClipboardMonitorClass;

struct KrakenClipboardInfo {
	GList *files;
	gboolean cut;
};

GType   kraken_clipboard_monitor_get_type (void);

KrakenClipboardMonitor *   kraken_clipboard_monitor_get (void);
void kraken_clipboard_monitor_set_clipboard_info (KrakenClipboardMonitor *monitor,
                                                    KrakenClipboardInfo *info);
KrakenClipboardInfo * kraken_clipboard_monitor_get_clipboard_info (KrakenClipboardMonitor *monitor);
void kraken_clipboard_monitor_emit_changed (void);

void kraken_clear_clipboard_callback (GtkClipboard *clipboard,
                                        gpointer      user_data);
void kraken_get_clipboard_callback   (GtkClipboard     *clipboard,
                                        GtkSelectionData *selection_data,
                                        guint             info,
                                        gpointer          user_data);



#endif /* KRAKEN_CLIPBOARD_MONITOR_H */

