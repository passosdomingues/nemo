/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * Copyright (C) 2005 Novell, Inc.
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
 * Author: Anders Carlsson <andersca@imendio.com>
 *
 */

#ifndef KRAKEN_QUERY_H
#define KRAKEN_QUERY_H

#include <glib-object.h>

#define KRAKEN_TYPE_QUERY		(kraken_query_get_type ())
#define KRAKEN_QUERY(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_QUERY, KrakenQuery))
#define KRAKEN_QUERY_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_QUERY, KrakenQueryClass))
#define KRAKEN_IS_QUERY(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_QUERY))
#define KRAKEN_IS_QUERY_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_QUERY))
#define KRAKEN_QUERY_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_QUERY, KrakenQueryClass))

typedef struct KrakenQueryDetails KrakenQueryDetails;

typedef struct KrakenQuery {
	GObject parent;
	KrakenQueryDetails *details;
} KrakenQuery;

typedef struct {
	GObjectClass parent_class;
} KrakenQueryClass;

GType          kraken_query_get_type (void);
gboolean       kraken_query_enabled  (void);

KrakenQuery* kraken_query_new      (void);

char *         kraken_query_get_file_pattern (KrakenQuery *query);
void           kraken_query_set_file_pattern (KrakenQuery *query, const char *text);

char *         kraken_query_get_content_pattern (KrakenQuery *query);
void           kraken_query_set_content_pattern (KrakenQuery *query, const char *text);
gboolean       kraken_query_has_content_pattern (KrakenQuery *query);

char *         kraken_query_get_location       (KrakenQuery *query);
void           kraken_query_set_location       (KrakenQuery *query, const char *uri);

GList *        kraken_query_get_mime_types     (KrakenQuery *query);
void           kraken_query_set_mime_types     (KrakenQuery *query, GList *mime_types);
void           kraken_query_add_mime_type      (KrakenQuery *query, const char *mime_type);

void           kraken_query_set_show_hidden    (KrakenQuery *query, gboolean hidden);
gboolean       kraken_query_get_show_hidden    (KrakenQuery *query);

gboolean       kraken_query_get_file_case_sensitive (KrakenQuery *query);
void           kraken_query_set_file_case_sensitive (KrakenQuery *query, gboolean case_sensitive);

gboolean       kraken_query_get_content_case_sensitive (KrakenQuery *query);
void           kraken_query_set_content_case_sensitive (KrakenQuery *query, gboolean case_sensitive);

gboolean       kraken_query_get_use_file_regex      (KrakenQuery *query);
void           kraken_query_set_use_file_regex      (KrakenQuery *query, gboolean file_use_regex);

gboolean       kraken_query_get_use_content_regex      (KrakenQuery *query);
void           kraken_query_set_use_content_regex      (KrakenQuery *query, gboolean content_use_regex);

gboolean       kraken_query_get_recurse         (KrakenQuery *query);
void           kraken_query_set_recurse         (KrakenQuery *query, gboolean recurse);

gboolean       kraken_query_get_semantic        (KrakenQuery *query);
void           kraken_query_set_semantic        (KrakenQuery *query, gboolean semantic);

char *         kraken_query_to_readable_string (KrakenQuery *query);
KrakenQuery *kraken_query_load               (char *file);
gboolean       kraken_query_save               (KrakenQuery *query, char *file);

#endif /* KRAKEN_QUERY_H */
