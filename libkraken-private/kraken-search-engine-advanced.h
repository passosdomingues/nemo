/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
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
 */

#ifndef KRAKEN_SEARCH_ENGINE_ADVANCED_H
#define KRAKEN_SEARCH_ENGINE_ADVANCED_H

#include <libkraken-private/kraken-search-engine.h>

#define KRAKEN_TYPE_SEARCH_ENGINE_ADVANCED		(kraken_search_engine_advanced_get_type ())
#define KRAKEN_SEARCH_ENGINE_ADVANCED(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_SEARCH_ENGINE_ADVANCED, KrakenSearchEngineAdvanced))
#define KRAKEN_SEARCH_ENGINE_ADVANCED_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_SEARCH_ENGINE_ADVANCED, KrakenSearchEngineAdvancedClass))
#define KRAKEN_IS_SEARCH_ENGINE_SIMPLE(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_SEARCH_ENGINE_ADVANCED))
#define KRAKEN_IS_SEARCH_ENGINE_SIMPLE_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_SEARCH_ENGINE_ADVANCED))
#define KRAKEN_SEARCH_ENGINE_ADVANCED_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_SEARCH_ENGINE_ADVANCED, KrakenSearchEngineAdvancedClass))

typedef struct KrakenSearchEngineAdvancedDetails KrakenSearchEngineAdvancedDetails;

typedef struct KrakenSearchEngineAdvanced {
	KrakenSearchEngine parent;
	KrakenSearchEngineAdvancedDetails *details;
} KrakenSearchEngineAdvanced;

typedef struct {
	KrakenSearchEngineClass parent_class;
} KrakenSearchEngineAdvancedClass;

GType          kraken_search_engine_advanced_get_type  (void);

KrakenSearchEngine* kraken_search_engine_advanced_new       (void);
void           free_search_helpers (void);

gboolean kraken_search_engine_advanced_check_filename_pattern (KrakenQuery   *query,
                                                             GError     **error);
gboolean kraken_search_engine_advanced_check_content_pattern  (KrakenQuery *query,
                                                             GError   **error);
#endif /* KRAKEN_SEARCH_ENGINE_ADVANCED_H */
