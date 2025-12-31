/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * kraken-application: main Kraken application class.
 *
 * Copyright (C) 2000 Red Hat, Inc.
 * Copyright (C) 2010 Cosimo Cecchi <cosimoc@gnome.org>
 *
 * Kraken is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * Kraken is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street - Suite 500,
 * Boston, MA 02110-1335, USA.
 */

#ifndef __KRAKEN_APPLICATION_H__
#define __KRAKEN_APPLICATION_H__

#include <gdk/gdk.h>
#include <gio/gio.h>
#include <gtk/gtk.h>

#include <libkraken-private/kraken-undo-manager.h>

#include "kraken-window.h"

#define KRAKEN_TYPE_APPLICATION kraken_application_get_type()
#define KRAKEN_APPLICATION(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_APPLICATION, KrakenApplication))
#define KRAKEN_APPLICATION_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_APPLICATION, KrakenApplicationClass))
#define KRAKEN_IS_APPLICATION(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_APPLICATION))
#define KRAKEN_IS_APPLICATION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_APPLICATION))
#define KRAKEN_APPLICATION_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_APPLICATION, KrakenApplicationClass))

typedef struct _KrakenApplicationPriv KrakenApplicationPriv;
typedef struct KrakenApplicationClass KrakenApplicationClass;

typedef struct {
	GtkApplication parent;

	KrakenUndoManager *undo_manager;

	KrakenApplicationPriv *priv;
} KrakenApplication;

struct KrakenApplicationClass {
	GtkApplicationClass parent_class;

    void         (* continue_startup) (KrakenApplication *application);
    void         (* continue_quit) (KrakenApplication *application);

    void         (* open_location) (KrakenApplication *application,
                                    GFile *location,
                                    GFile *selection,
                                    const char *startup_id,
                                    const gboolean open_in_tabs);

    KrakenWindow * (* create_window) (KrakenApplication *application,
                                    GdkScreen       *screen);

    void         (* notify_unmount_done) (KrakenApplication *application,
                                          const gchar *message);

    void         (* notify_unmount_show) (KrakenApplication *application,
                                          const gchar *message);

    void         (* close_all_windows)   (KrakenApplication *application);

};

GType kraken_application_get_type (void);
KrakenApplication *kraken_application_initialize_singleton (GType object_type,
                                                        const gchar *first_property_name,
                                                        ...);
KrakenApplication *kraken_application_get_singleton (void);
void kraken_application_quit (KrakenApplication *self);
KrakenWindow *     kraken_application_create_window (KrakenApplication *application,
                                                 GdkScreen           *screen);
void kraken_application_open_location (KrakenApplication *application,
                                     GFile *location,
                                     GFile *selection,
                                     const char *startup_id,
                                     const gboolean open_in_tabs);
void kraken_application_close_all_windows (KrakenApplication *self);

void kraken_application_notify_unmount_show (KrakenApplication *application,
                                               const gchar *message);

void kraken_application_notify_unmount_done (KrakenApplication *application,
                                               const gchar *message);
gboolean kraken_application_check_required_directory (KrakenApplication *application,
                                                    gchar           *path);
void kraken_application_check_thumbnail_cache (KrakenApplication *application);
gboolean kraken_application_get_cache_bad (KrakenApplication *application);
void kraken_application_clear_cache_flag (KrakenApplication *application);
void kraken_application_set_cache_flag (KrakenApplication *application);
void kraken_application_ignore_cache_problem (KrakenApplication *application);
gboolean kraken_application_get_cache_problem_ignored (KrakenApplication *application);
gboolean kraken_application_get_show_desktop (KrakenApplication *application);

#endif /* __KRAKEN_APPLICATION_H__ */
