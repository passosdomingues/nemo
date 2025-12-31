/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * Copyright (C) 2005 Mr Jamie McCracken
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
 * Author: Jamie McCracken (jamiemcc@gnome.org)
 *
 */

#ifndef KRAKEN_SEARCH_ENGINE_TRACKER_H
#define KRAKEN_SEARCH_ENGINE_TRACKER_H

#include <libkraken-private/kraken-search-engine.h>

#define KRAKEN_TYPE_SEARCH_ENGINE_TRACKER		(kraken_search_engine_tracker_get_type ())
#define KRAKEN_SEARCH_ENGINE_TRACKER(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_SEARCH_ENGINE_TRACKER, KrakenSearchEngineTracker))
#define KRAKEN_SEARCH_ENGINE_TRACKER_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_SEARCH_ENGINE_TRACKER, KrakenSearchEngineTrackerClass))
#define KRAKEN_IS_SEARCH_ENGINE_TRACKER(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_SEARCH_ENGINE_TRACKER))
#define KRAKEN_IS_SEARCH_ENGINE_TRACKER_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_SEARCH_ENGINE_TRACKER))
#define KRAKEN_SEARCH_ENGINE_TRACKER_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_SEARCH_ENGINE_TRACKER, KrakenSearchEngineTrackerClass))

typedef struct KrakenSearchEngineTrackerDetails KrakenSearchEngineTrackerDetails;

typedef struct KrakenSearchEngineTracker {
	KrakenSearchEngine parent;
	KrakenSearchEngineTrackerDetails *details;
} KrakenSearchEngineTracker;

typedef struct {
	KrakenSearchEngineClass parent_class;
} KrakenSearchEngineTrackerClass;

GType kraken_search_engine_tracker_get_type (void);

KrakenSearchEngine* kraken_search_engine_tracker_new (void);

#endif /* KRAKEN_SEARCH_ENGINE_TRACKER_H */
