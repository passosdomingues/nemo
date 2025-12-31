/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
#include <config.h>
#include "kraken-search-engine-kraken.h"
#include "../kraken-core/kraken-daemon.h"
#include <gio/gio.h>
#include <string.h>

struct KrakenSearchEngineKrakenDetails {
	KrakenQuery *query;
	kraken_daemon_t *daemon;
	GCancellable *cancellable;
	gboolean query_pending;
};

G_DEFINE_TYPE (KrakenSearchEngineKraken, kraken_search_engine_kraken, KRAKEN_TYPE_SEARCH_ENGINE);

static void finalize (GObject *object) {
	KrakenSearchEngineKraken *self = KRAKEN_SEARCH_ENGINE_KRAKEN (object);
	g_clear_object (&self->details->query);
	g_clear_object (&self->details->cancellable);
	if (self->details->daemon) {
		kraken_daemon_free (self->details->daemon);
	}
	G_OBJECT_CLASS (kraken_search_engine_kraken_parent_class)->finalize (object);
}

static void on_search_done (GObject *source, GAsyncResult *res, gpointer user_data) {
	KrakenSearchEngineKraken *self = KRAKEN_SEARCH_ENGINE_KRAKEN (user_data);
	GError *error = NULL;
	GList *results = kraken_daemon_search_finish (self->details->daemon, res, &error);

	self->details->query_pending = FALSE;

	if (error) {
		kraken_search_engine_error (KRAKEN_SEARCH_ENGINE (self), error->message);
		g_error_free (error);
	} else {
		GList *hits = NULL;
		for (GList *l = results; l != NULL; l = l->next) {
			SearchResult *sr = (SearchResult*)l->data;
			char *uri = g_filename_to_uri (sr->path, NULL, NULL);
			if (uri) {
				FileSearchResult *fsr = file_search_result_new (uri, NULL);
				hits = g_list_append (hits, fsr);
			}
			g_free (sr->path);
			g_free (sr);
		}
		g_list_free (results);
		
		kraken_search_engine_hits_added (KRAKEN_SEARCH_ENGINE (self), hits);
		kraken_search_engine_finished (KRAKEN_SEARCH_ENGINE (self));
	}
	g_object_unref (self);
}

static void kraken_search_engine_kraken_start (KrakenSearchEngine *engine) {
	KrakenSearchEngineKraken *self = KRAKEN_SEARCH_ENGINE_KRAKEN (engine);
	char *query_text;

	if (self->details->query_pending || !self->details->query) return;

	query_text = kraken_query_get_file_pattern (self->details->query);
	if (!query_text || strlen(query_text) == 0) {
		g_free (query_text);
		kraken_search_engine_finished (engine);
		return;
	}

	self->details->query_pending = TRUE;
	g_object_ref (self);
	kraken_daemon_search_async (self->details->daemon, query_text, on_search_done, self);
	g_free (query_text);
}

static void kraken_search_engine_kraken_stop (KrakenSearchEngine *engine) {
	KrakenSearchEngineKraken *self = KRAKEN_SEARCH_ENGINE_KRAKEN (engine);
	if (self->details->query_pending) {
		g_cancellable_cancel (self->details->cancellable);
		self->details->query_pending = FALSE;
	}
}

static void kraken_search_engine_kraken_set_query (KrakenSearchEngine *engine, KrakenQuery *query) {
	KrakenSearchEngineKraken *self = KRAKEN_SEARCH_ENGINE_KRAKEN (engine);
	if (query) g_object_ref (query);
	if (self->details->query) g_object_unref (self->details->query);
	self->details->query = query;
}

static void kraken_search_engine_kraken_class_init (KrakenSearchEngineKrakenClass *klass) {
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	KrakenSearchEngineClass *engine_class = KRAKEN_SEARCH_ENGINE_CLASS (klass);

	gobject_class->finalize = finalize;
	engine_class->set_query = kraken_search_engine_kraken_set_query;
	engine_class->start = kraken_search_engine_kraken_start;
	engine_class->stop = kraken_search_engine_kraken_stop;

	g_type_class_add_private (klass, sizeof (KrakenSearchEngineKrakenDetails));
}

static void kraken_search_engine_kraken_init (KrakenSearchEngineKraken *self) {
	self->details = G_TYPE_INSTANCE_GET_PRIVATE (self, KRAKEN_TYPE_SEARCH_ENGINE_KRAKEN, KrakenSearchEngineKrakenDetails);
	self->details->daemon = kraken_daemon_new ();
	self->details->cancellable = g_cancellable_new ();
}

KrakenSearchEngine* kraken_search_engine_kraken_new (void) {
	return g_object_new (KRAKEN_TYPE_SEARCH_ENGINE_KRAKEN, NULL);
}
