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

#ifndef KRAKEN_SEARCH_ENGINE_H
#define KRAKEN_SEARCH_ENGINE_H

#include <glib-object.h>
#include <libkraken-private/kraken-query.h>

#define KRAKEN_TYPE_SEARCH_ENGINE		(kraken_search_engine_get_type ())
#define KRAKEN_SEARCH_ENGINE(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_SEARCH_ENGINE, KrakenSearchEngine))
#define KRAKEN_SEARCH_ENGINE_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_SEARCH_ENGINE, KrakenSearchEngineClass))
#define KRAKEN_IS_SEARCH_ENGINE(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_SEARCH_ENGINE))
#define KRAKEN_IS_SEARCH_ENGINE_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_SEARCH_ENGINE))
#define KRAKEN_SEARCH_ENGINE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_SEARCH_ENGINE, KrakenSearchEngineClass))

typedef struct KrakenSearchEngineDetails KrakenSearchEngineDetails;

typedef struct KrakenSearchEngine {
	GObject parent;
	KrakenSearchEngineDetails *details;
} KrakenSearchEngine;

typedef struct {
	GObjectClass parent_class;
	
	/* VTable */
	void (*set_query) (KrakenSearchEngine *engine, KrakenQuery *query);
	void (*start) (KrakenSearchEngine *engine);
	void (*stop) (KrakenSearchEngine *engine);

	/* Signals */
	void (*hits_added) (KrakenSearchEngine *engine, GList *hit_infos);
	void (*hits_subtracted) (KrakenSearchEngine *engine, GList *hits);
	void (*finished) (KrakenSearchEngine *engine);
	void (*error) (KrakenSearchEngine *engine, const char *error_message);
} KrakenSearchEngineClass;

GType          kraken_search_engine_get_type  (void);
gboolean       kraken_search_engine_enabled (void);

KrakenSearchEngine* kraken_search_engine_new       (void);

void           kraken_search_engine_set_query (KrakenSearchEngine *engine, KrakenQuery *query);
void	       kraken_search_engine_start (KrakenSearchEngine *engine);
void	       kraken_search_engine_stop (KrakenSearchEngine *engine);

void	       kraken_search_engine_hits_added (KrakenSearchEngine *engine, GList *hits);
void	       kraken_search_engine_hits_subtracted (KrakenSearchEngine *engine, GList *hits);
void	       kraken_search_engine_finished (KrakenSearchEngine *engine);
void	       kraken_search_engine_error (KrakenSearchEngine *engine, const char *error_message);

typedef struct {
    gchar     *uri;           // The file uri;
    gchar     *snippet;          // List of hits.
    gint64     hits;
} FileSearchResult;

FileSearchResult *file_search_result_new     (gchar *uri, gchar *snippet);
void              file_search_result_free    (FileSearchResult *result);
void              file_search_result_add_hit (FileSearchResult *result);

gboolean       kraken_search_engine_check_filename_pattern (KrakenQuery   *query,
                                                          GError     **error);
gboolean       kraken_search_engine_check_content_pattern  (KrakenQuery   *query,
                                                          GError     **error);

void              kraken_search_engine_report_accounting (void);
#endif /* KRAKEN_SEARCH_ENGINE_H */
