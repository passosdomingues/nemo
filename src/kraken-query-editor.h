/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
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
 * You should have received a copy of the GNU General Public
 * License along with this program; see the file COPYING.  If not,
 * write to the Free Software Foundation, Inc., 51 Franklin Street - Suite 500,
 * Boston, MA 02110-1335, USA.
 *
 * Author: Alexander Larsson <alexl@redhat.com>
 *
 */

#ifndef KRAKEN_QUERY_EDITOR_H
#define KRAKEN_QUERY_EDITOR_H

#include <gtk/gtk.h>

#include <libkraken-private/kraken-query.h>

G_BEGIN_DECLS

#define KRAKEN_TYPE_QUERY_EDITOR (kraken_query_editor_get_type ())

G_DECLARE_FINAL_TYPE (KrakenQueryEditor, kraken_query_editor, KRAKEN, QUERY_EDITOR, GtkBox)

GtkWidget* kraken_query_editor_new                (void);

KrakenQuery   *kraken_query_editor_get_query          (KrakenQueryEditor *editor);
void         kraken_query_editor_set_query          (KrakenQueryEditor *editor,
                                                   KrakenQuery       *query);
GFile       *kraken_query_editor_get_location       (KrakenQueryEditor *editor);
void         kraken_query_editor_set_location       (KrakenQueryEditor *editor,
                                                   GFile           *location);
void         kraken_query_editor_set_active         (KrakenQueryEditor *editor,
                                                   gchar           *base_uri,
                                                   gboolean         active);
gboolean     kraken_query_editor_get_active         (KrakenQueryEditor *editor);
const gchar *kraken_query_editor_get_base_uri       (KrakenQueryEditor *editor);

G_END_DECLS

#endif /* KRAKEN_QUERY_EDITOR_H */
