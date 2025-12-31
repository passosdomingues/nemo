/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
#ifndef KRAKEN_SEARCH_ENGINE_KRAKEN_H
#define KRAKEN_SEARCH_ENGINE_KRAKEN_H

#include <libkraken-private/kraken-search-engine.h>

#define KRAKEN_TYPE_SEARCH_ENGINE_KRAKEN (kraken_search_engine_kraken_get_type ())
#define KRAKEN_SEARCH_ENGINE_KRAKEN(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_SEARCH_ENGINE_KRAKEN, KrakenSearchEngineKraken))
#define KRAKEN_SEARCH_ENGINE_KRAKEN_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_SEARCH_ENGINE_KRAKEN, KrakenSearchEngineKrakenClass))
#define KRAKEN_IS_SEARCH_ENGINE_KRAKEN(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_SEARCH_ENGINE_KRAKEN))
#define KRAKEN_IS_SEARCH_ENGINE_KRAKEN_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_SEARCH_ENGINE_KRAKEN))
#define KRAKEN_SEARCH_ENGINE_KRAKEN_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_SEARCH_ENGINE_KRAKEN, KrakenSearchEngineKrakenClass))

typedef struct KrakenSearchEngineKrakenDetails KrakenSearchEngineKrakenDetails;

typedef struct {
	KrakenSearchEngine parent;
	KrakenSearchEngineKrakenDetails *details;
} KrakenSearchEngineKraken;

typedef struct {
	KrakenSearchEngineClass parent_class;
} KrakenSearchEngineKrakenClass;

GType kraken_search_engine_kraken_get_type (void);

KrakenSearchEngine* kraken_search_engine_kraken_new (void);

#endif /* KRAKEN_SEARCH_ENGINE_KRAKEN_H */
