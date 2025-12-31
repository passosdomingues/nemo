#ifndef KRAKEN_DAEMON_H
#define KRAKEN_DAEMON_H

#include <glib.h>
#include <gio/gio.h>
#include <sqlite3.h>
#include "../kraken-lib/kraken-llm.h"

typedef struct {
    char* path;
    long timestamp_ns;
    char* mime_type;
    char* actor;       /* who created/modified */
    float confidence;  /* provenance */
} kraken_event_t;

typedef struct {
    GMainLoop* main_loop;
    GList* event_queue;
    sqlite3* db;
    GFileMonitor* monitor;
    kraken_llm_t* llm;
} kraken_daemon_t;

typedef struct {
    char* path;
    float score;
} SearchResult;

kraken_daemon_t* kraken_daemon_new(void);
void kraken_daemon_run(kraken_daemon_t* self);
void kraken_daemon_stop(kraken_daemon_t* self);
void kraken_daemon_free(kraken_daemon_t* self);

/* Semantic Search API */
void kraken_daemon_search_async(kraken_daemon_t *self, const char *query, GAsyncReadyCallback callback, gpointer user_data);
GList* kraken_daemon_search_finish(kraken_daemon_t *self, GAsyncResult *res, GError **error);

/* Associative Tagging API */
void kraken_daemon_get_tags_async(kraken_daemon_t *self, GAsyncReadyCallback callback, gpointer user_data);
GList* kraken_daemon_get_tags_finish(kraken_daemon_t *self, GAsyncResult *res, GError **error);

void kraken_daemon_get_files_by_tag_async(kraken_daemon_t *self, const char *tag_name, GAsyncReadyCallback callback, gpointer user_data);
GList* kraken_daemon_get_files_by_tag_finish(kraken_daemon_t *self, GAsyncResult *res, GError **error);

/* Graph Relationship API */
typedef struct {
    char *file_path;
    char *related_path;
    float weight;        // 0.0 to 1.0
    int relationship_type; // 0=tag, 1=semantic, 2=temporal
} FileRelationship;

void kraken_daemon_get_related_files_async(kraken_daemon_t *self, const char *file_path, GAsyncReadyCallback callback, gpointer user_data);
GList* kraken_daemon_get_related_files_finish(kraken_daemon_t *self, GAsyncResult *res, GError **error);

#endif /* KRAKEN_DAEMON_H */
