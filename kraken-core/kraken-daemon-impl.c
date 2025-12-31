#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "kraken-daemon.h"
#include "../kraken-lib/toon.h"

static void kraken_daemon_update_event_summary(kraken_daemon_t* self, const char* path, const char* summary) {
    const char* sql = "UPDATE events SET toon_payload = json_set(toon_payload, '$.summary', ?) WHERE path = ? AND id = (SELECT max(id) FROM events WHERE path = ?);";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(self->db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        g_printerr("Kraken Daemon: Failed to prepare update statement: %s\n", sqlite3_errmsg(self->db));
        return;
    }
    
    sqlite3_bind_text(stmt, 1, summary, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, path, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, path, -1, SQLITE_TRANSIENT);
    
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        g_printerr("Kraken Daemon: Failed to update event summary: %s\n", sqlite3_errmsg(self->db));
    }
    sqlite3_finalize(stmt);
}

static void kraken_daemon_store_event(kraken_daemon_t* self, const char* path, GFileMonitorEvent event_type, const char* toon_payload, const char* snapshot_path) {
    const char* sql = "INSERT INTO events (path, timestamp, mime_type, actor, confidence, toon_payload, snapshot_path) VALUES (?, ?, ?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(self->db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        g_printerr("Kraken Daemon: Failed to prepare statement: %s\n", sqlite3_errmsg(self->db));
        return;
    }
    
    sqlite3_bind_text(stmt, 1, path, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 2, g_get_real_time());
    sqlite3_bind_text(stmt, 3, "unknown", -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, g_get_user_name(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 5, 1.0);
    sqlite3_bind_text(stmt, 6, toon_payload, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 7, snapshot_path, -1, SQLITE_TRANSIENT);
    
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        g_printerr("Kraken Daemon: Failed to insert event: %s\n", sqlite3_errmsg(self->db));
    }
    
    sqlite3_finalize(stmt);
}

static void kraken_daemon_store_embedding(kraken_daemon_t* self, const char* path, float* vector, int dim) {
    const char* sql = "INSERT INTO embeddings (path, vector) VALUES (?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(self->db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, path, -1, SQLITE_TRANSIENT);
        sqlite3_bind_blob(stmt, 2, vector, dim * sizeof(float), SQLITE_TRANSIENT);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        g_print("Kraken Daemon: Embedding stored for %s\n", path);
        fflush(stdout);
    } else {
        g_printerr("Kraken Daemon: Failed to prepare embedding storage: %s\n", sqlite3_errmsg(self->db));
        fflush(stderr);
    }
}

static void kraken_daemon_store_tags(kraken_daemon_t* self, const char* path, char** tags) {
    if (!tags) return;
    
    for (int i = 0; tags[i] != NULL; i++) {
        const char* tag_name = tags[i];
        if (strlen(tag_name) == 0) continue;
        
        sqlite3_stmt* tag_stmt;
        if (sqlite3_prepare_v2(self->db, "INSERT OR IGNORE INTO tags (name) VALUES (?);", -1, &tag_stmt, NULL) == SQLITE_OK) {
            sqlite3_bind_text(tag_stmt, 1, tag_name, -1, SQLITE_TRANSIENT);
            sqlite3_step(tag_stmt);
            sqlite3_finalize(tag_stmt);
        }
        
        sqlite3_stmt* link_stmt;
        if (sqlite3_prepare_v2(self->db, "INSERT OR IGNORE INTO file_tags (path, tag_id) SELECT ?, id FROM tags WHERE name = ?;", -1, &link_stmt, NULL) == SQLITE_OK) {
            sqlite3_bind_text(link_stmt, 1, path, -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(link_stmt, 2, tag_name, -1, SQLITE_TRANSIENT);
            sqlite3_step(link_stmt);
            sqlite3_finalize(link_stmt);
        }
    }
}

static float cosine_similarity(float* a, float* b, int dim) {
    double dot = 0.0, denom_a = 0.0, denom_b = 0.0;
    for (int i = 0; i < dim; i++) {
        dot += a[i] * b[i];
        denom_a += a[i] * a[i];
        denom_b += b[i] * b[i];
    }
    if (denom_a == 0 || denom_b == 0) return 0;
    return (float)(dot / (sqrt(denom_a) * sqrt(denom_b)));
}

static gint compare_search_results(gconstpointer a, gconstpointer b) {
    SearchResult* res_a = (SearchResult*)a;
    SearchResult* res_b = (SearchResult*)b;
    if (res_a->score < res_b->score) return 1;
    if (res_a->score > res_b->score) return -1;
    return 0;
}

typedef struct {
    kraken_daemon_t* daemon;
    char* query;
} SearchTaskData;

static void search_task_data_free(SearchTaskData* data) {
    g_free(data->query);
    g_free(data);
}

static void on_search_query_embed_done(GObject* source, GAsyncResult* res, gpointer user_data) {
    GTask* task = G_TASK(user_data);
    SearchTaskData* td = (SearchTaskData*)g_task_get_task_data(task);
    kraken_daemon_t* self = td->daemon;
    GError* error = NULL;
    int dim = 0;
    float* query_vector = kraken_llm_embed_finish(self->llm, res, &dim, &error);
    
    if (error) {
        g_printerr("Kraken Daemon: Embedding failed (LLM offline?), proceeding with keyword search only: %s\n", error->message);
        g_clear_error(&error);
    }
    
    GHashTable* hit_map = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
    
    /* 1. Keyword search pass */
    const char* kw_sql = "SELECT DISTINCT path FROM events WHERE toon_payload LIKE ?;";
    sqlite3_stmt* kw_stmt;
    if (sqlite3_prepare_v2(self->db, kw_sql, -1, &kw_stmt, NULL) == SQLITE_OK) {
        char* pattern = g_strdup_printf("%%%s%%", td->query);
        sqlite3_bind_text(kw_stmt, 1, pattern, -1, g_free);
        while (sqlite3_step(kw_stmt) == SQLITE_ROW) {
            const char* path = (const char*)sqlite3_column_text(kw_stmt, 0);
            SearchResult* sr = g_new0(SearchResult, 1);
            sr->path = g_strdup(path);
            sr->score = 0.4f; /* Keyword match base score */
            g_hash_table_insert(hit_map, g_strdup(path), sr);
        }
        sqlite3_finalize(kw_stmt);
    }
    
    /* 2. Vector search pass */
    const char* vec_sql = "SELECT path, vector FROM embeddings;";
    sqlite3_stmt* vec_stmt;
    if (sqlite3_prepare_v2(self->db, vec_sql, -1, &vec_stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(vec_stmt) == SQLITE_ROW) {
            const char* path = (const char*)sqlite3_column_text(vec_stmt, 0);
            const void* blob = sqlite3_column_blob(vec_stmt, 1);
            int blob_size = sqlite3_column_bytes(vec_stmt, 1);
            
            if (blob_size == dim * sizeof(float) && query_vector) {
                float similarity = cosine_similarity(query_vector, (float*)blob, dim);
                if (similarity > 0.5) {
                    SearchResult* sr = g_hash_table_lookup(hit_map, path);
                    if (sr) {
                        /* Boost existing result */
                        sr->score += similarity * 0.6f;
                    } else {
                        sr = g_new0(SearchResult, 1);
                        sr->path = g_strdup(path);
                        sr->score = similarity * 0.6f;
                        g_hash_table_insert(hit_map, g_strdup(path), sr);
                    }
                }
            }
        }
        sqlite3_finalize(vec_stmt);
    }
    
    /* 3. Collect and sort results */
    GList* results = NULL;
    GList* hash_values = g_hash_table_get_values(hit_map);
    for (GList* l = hash_values; l != NULL; l = l->next) {
        SearchResult* sr = (SearchResult*)l->data;
        /* We need to copy because the hash table owns the original sr if not careful, 
           but here we'll just steal them and destroy the container. */
        results = g_list_insert_sorted(results, sr, compare_search_results);
    }
    g_list_free(hash_values);
    
    /* Clear hash table without freeing values as we moved them to the list */
    g_hash_table_steal_all(hit_map);
    g_hash_table_destroy(hit_map);
    
    g_free(query_vector);
    g_task_return_pointer(task, results, NULL);
    g_object_unref(task);
}

void kraken_daemon_search_async(kraken_daemon_t *self, const char *query, GAsyncReadyCallback callback, gpointer user_data) {
    GTask* task = g_task_new(NULL, NULL, callback, user_data);
    SearchTaskData* td = g_new0(SearchTaskData, 1);
    td->daemon = self;
    td->query = g_strdup(query);
    g_task_set_task_data(task, td, (GDestroyNotify)search_task_data_free);
    kraken_llm_embed_async(self->llm, NULL, query, on_search_query_embed_done, task);
}

GList* kraken_daemon_search_finish(kraken_daemon_t *self, GAsyncResult *res, GError **error) {
    return g_task_propagate_pointer(G_TASK(res), error);
}

static void on_get_tags_thread(GTask *task, gpointer source_object, gpointer task_data, GCancellable *cancellable) {
    kraken_daemon_t* self = (kraken_daemon_t*)task_data;
    const char* sql = "SELECT name FROM tags ORDER BY name ASC;";
    sqlite3_stmt* stmt;
    GList* tags = NULL;
    
    if (sqlite3_prepare_v2(self->db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            tags = g_list_append(tags, g_strdup((const char*)sqlite3_column_text(stmt, 0)));
        }
        sqlite3_finalize(stmt);
    }
    g_task_return_pointer(task, tags, (GDestroyNotify)NULL);
}

void kraken_daemon_get_tags_async(kraken_daemon_t *self, GAsyncReadyCallback callback, gpointer user_data) {
    GTask* task = g_task_new(NULL, NULL, callback, user_data);
    g_task_set_task_data(task, self, NULL);
    g_task_run_in_thread(task, (GTaskThreadFunc)on_get_tags_thread);
    g_object_unref(task);
}

GList* kraken_daemon_get_tags_finish(kraken_daemon_t *self, GAsyncResult *res, GError **error) {
    return g_task_propagate_pointer(G_TASK(res), error);
}

static void on_get_files_by_tag_thread(GTask *task, gpointer source_object, gpointer task_data, GCancellable *cancellable) {
    SearchTaskData* td = (SearchTaskData*)task_data;
    kraken_daemon_t* self = td->daemon;
    const char* sql = "SELECT path FROM file_tags "
                      "JOIN tags ON file_tags.tag_id = tags.id "
                      "WHERE tags.name = ?;";
    sqlite3_stmt* stmt;
    GList* results = NULL;
    
    if (sqlite3_prepare_v2(self->db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, td->query, -1, SQLITE_TRANSIENT);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            SearchResult* sr = g_new0(SearchResult, 1);
            sr->path = g_strdup((const char*)sqlite3_column_text(stmt, 0));
            sr->score = 1.0; 
            results = g_list_append(results, sr);
        }
        sqlite3_finalize(stmt);
    }
    g_task_return_pointer(task, results, (GDestroyNotify)NULL);
}

void kraken_daemon_get_files_by_tag_async(kraken_daemon_t *self, const char *tag_name, GAsyncReadyCallback callback, gpointer user_data) {
    GTask* task = g_task_new(NULL, NULL, callback, user_data);
    SearchTaskData* td = g_new0(SearchTaskData, 1);
    td->daemon = self;
    td->query = g_strdup(tag_name);
    g_task_set_task_data(task, td, (GDestroyNotify)search_task_data_free);
    g_task_run_in_thread(task, (GTaskThreadFunc)on_get_files_by_tag_thread);
    g_object_unref(task);
}

GList* kraken_daemon_get_files_by_tag_finish(kraken_daemon_t *self, GAsyncResult *res, GError **error) {
    return g_task_propagate_pointer(G_TASK(res), error);
}

/* Graph Relationship API Implementation */
static void on_get_related_files_thread(GTask *task, gpointer source_object, gpointer task_data, GCancellable *cancellable) {
    SearchTaskData* td = (SearchTaskData*)task_data;
    kraken_daemon_t* self = td->daemon;
    GList* relationships = NULL;
    
    // Get files with shared tags
    const char* tag_sql = "SELECT DISTINCT ft2.path, COUNT(*) as shared_tags "
                          "FROM file_tags ft1 "
                          "JOIN file_tags ft2 ON ft1.tag_id = ft2.tag_id "
                          "WHERE ft1.path = ? AND ft2.path != ? "
                          "GROUP BY ft2.path;";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(self->db, tag_sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, td->query, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, td->query, -1, SQLITE_TRANSIENT);
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            FileRelationship* rel = g_new0(FileRelationship, 1);
            rel->file_path = g_strdup(td->query);
            rel->related_path = g_strdup((const char*)sqlite3_column_text(stmt, 0));
            int shared_count = sqlite3_column_int(stmt, 1);
            rel->weight = (float)shared_count / 5.0; // Normalize by max expected tags
            if (rel->weight > 1.0) rel->weight = 1.0;
            rel->relationship_type = 0; // TAG similarity
            relationships = g_list_append(relationships, rel);
        }
        sqlite3_finalize(stmt);
    }
    
    g_task_return_pointer(task, relationships, (GDestroyNotify)NULL);
}

void kraken_daemon_get_related_files_async(kraken_daemon_t *self, const char *file_path, GAsyncReadyCallback callback, gpointer user_data) {
    GTask* task = g_task_new(NULL, NULL, callback, user_data);
    SearchTaskData* td = g_new0(SearchTaskData, 1);
    td->daemon = self;
    td->query = g_strdup(file_path);
    g_task_set_task_data(task, td, (GDestroyNotify)search_task_data_free);
    g_task_run_in_thread(task, (GTaskThreadFunc)on_get_related_files_thread);
    g_object_unref(task);
}

GList* kraken_daemon_get_related_files_finish(kraken_daemon_t *self, GAsyncResult *res, GError **error) {
    return g_task_propagate_pointer(G_TASK(res), error);
}

static void on_embed_done(GObject* source, GAsyncResult* res, gpointer user_data) {
    kraken_daemon_t* daemon = (kraken_daemon_t*)user_data;
    kraken_llm_t* llm = daemon->llm;
    GError* error = NULL;
    int dim = 0;
    float* vector = kraken_llm_embed_finish(llm, res, &dim, &error);
    
    if (error) {
        g_printerr("Kraken Daemon: Embedding failed: %s\n", error->message);
        g_error_free(error);
    } else if (vector) {
        LLMTask* data = (LLMTask*)g_task_get_task_data(G_TASK(res));
        g_print("Kraken Daemon: Received vector (dim %d) for %s\n", dim, data ? data->file_path : "NULL");
        if (data && data->file_path) {
            kraken_daemon_store_embedding((kraken_daemon_t*)user_data, data->file_path, vector, dim);
        }
        g_free(vector);
    }
}

static void on_tags_extracted(GObject* source, GAsyncResult* res, gpointer user_data) {
    kraken_daemon_t* daemon = (kraken_daemon_t*)user_data;
    GError* error = NULL;
    char** tags = kraken_llm_extract_tags_finish(daemon->llm, res, &error);
    
    if (error) {
        g_printerr("Kraken Daemon: Tag extraction failed: %s\n", error->message);
        g_error_free(error);
    } else if (tags) {
        LLMTask* data = (LLMTask*)g_task_get_task_data(G_TASK(res));
        if (data && data->file_path) {
            g_print("Kraken Daemon: Tags extracted for %s: ", data->file_path);
            for (int i = 0; tags[i] != NULL; i++) g_print("[%s] ", tags[i]);
            g_print("\n");
            kraken_daemon_store_tags(daemon, data->file_path, tags);
        }
        g_strfreev(tags);
    }
}

static void on_summarize_done(GObject* source, GAsyncResult* res, gpointer user_data) {
    kraken_daemon_t* daemon = (kraken_daemon_t*)user_data;
    kraken_llm_t* llm = daemon->llm;
    GError* error = NULL;
    char* summary = kraken_llm_summarize_finish(llm, res, &error);
    
    if (error) {
        g_printerr("Kraken Daemon: Summarization failed: %s\n", error->message);
        g_error_free(error);
    } else if (summary) {
        /* Clean summary from LLM garbage tokens */
        char* clean_summary = g_strdup(summary);
        char* end_token = strstr(clean_summary, "<|");
        if (end_token) *end_token = '\0';
        g_strstrip(clean_summary);
        
        g_print("Kraken Daemon: Clean Summary: %s\n", clean_summary);
        
        /* Recover path from task data */
        LLMTask* data = (LLMTask*)g_task_get_task_data(G_TASK(res));
        if (data && data->file_path) {
             kraken_daemon_update_event_summary((kraken_daemon_t*)user_data, data->file_path, clean_summary);
             
             /* Now trigger embedding and tag extraction for the same file */
             g_print("Kraken Daemon: Triggering embedding & tagging for %s\n", data->file_path);
             kraken_llm_embed_async(llm, data->file_path, clean_summary, on_embed_done, user_data);
             kraken_llm_extract_tags_async(llm, data->file_path, clean_summary, on_tags_extracted, user_data);
        }
        g_free(clean_summary);
        g_free(summary);
    }
}

static void kraken_daemon_trigger_snapshot(kraken_daemon_t* self, const char* path, GFileMonitorEvent event_type) {
    long now = g_get_real_time();
    char* snapshot_path = g_strdup_printf("/home/rafael/KrakenDrive/snapshots/%ld", now);
    char* source_path = g_build_filename(g_get_home_dir(), "KrakenDrive", "data", NULL);
    
    char* cmd = g_strdup_printf("btrfs subvolume snapshot -r %s %s", source_path, snapshot_path);
    g_print("Kraken Daemon: Triggering BTRFS Snapshot: %s\n", cmd);
    
    GError* error = NULL;
    if (!g_spawn_command_line_sync(cmd, NULL, NULL, NULL, &error)) {
        g_printerr("Kraken Daemon: Snapshot failed: %s\n", error->message);
        g_error_free(error);
        kraken_daemon_store_event(self, path, event_type, NULL, NULL);
    } else {
        g_print("Kraken Daemon: Snapshot created at %s\n", snapshot_path);
        
        /* Generate TOON Metadata */
        kraken_toon_t* toon = kraken_toon_new(path);
        kraken_toon_add_relation(toon, "kraken:hasSnapshot", snapshot_path, 1.0);
        
        GEnumValue* ev = g_enum_get_value(g_type_class_ref(G_TYPE_FILE_MONITOR_EVENT), event_type);
        if (ev) {
            kraken_toon_add_relation(toon, "kraken:eventType", ev->value_name, 1.0);
        }
        
        char* toon_payload = kraken_toon_to_json(toon);
        kraken_daemon_store_event(self, path, event_type, toon_payload, snapshot_path);
        
        /* Queue LLM task */
        kraken_llm_summarize_async(self->llm, path, on_summarize_done, self);
        
        g_free(toon_payload);
        kraken_toon_free(toon);
    }
    
    g_free(cmd);
    g_free(source_path);
    g_free(snapshot_path);
}

static void kraken_daemon_on_file_changed(GFileMonitor* monitor,
                                          GFile* file,
                                          GFile* other_file,
                                          GFileMonitorEvent event_type,
                                          gpointer user_data) {
    kraken_daemon_t* self = (kraken_daemon_t*)user_data;
    char* path = g_file_get_path(file);
    g_print("Kraken Daemon Event: %s on %s\n", g_enum_get_value(g_type_class_ref(G_TYPE_FILE_MONITOR_EVENT), event_type)->value_name, path);
    
    /* Trigger snapshots on significant changes and store in DB */
    if (event_type == G_FILE_MONITOR_EVENT_CHANGES_DONE_HINT || 
        event_type == G_FILE_MONITOR_EVENT_MOVED_IN ||
        event_type == G_FILE_MONITOR_EVENT_CREATED) {
        kraken_daemon_trigger_snapshot(self, path, event_type);
    } else {
        kraken_daemon_store_event(self, path, event_type, NULL, NULL);
    }
    
    g_free(path);
}

static gboolean kraken_daemon_setup_monitoring(kraken_daemon_t* self) {
    char* watch_path = g_build_filename(g_get_home_dir(), "KrakenDrive", "data", NULL);
    GFile* file = g_file_new_for_path(watch_path);
    GError* error = NULL;
    
    self->monitor = g_file_monitor_directory(file, G_FILE_MONITOR_NONE, NULL, &error);
    if (error) {
        g_printerr("Kraken Daemon: Monitoring failed: %s\n", error->message);
        g_error_free(error);
        g_object_unref(file);
        g_free(watch_path);
        return FALSE;
    }
    
    g_signal_connect(self->monitor, "changed", G_CALLBACK(kraken_daemon_on_file_changed), self);
    g_print("Kraken Daemon: Monitoring directory %s\n", watch_path);
    
    g_object_unref(file);
    g_free(watch_path);
    return TRUE;
}

static gboolean kraken_daemon_init_db(kraken_daemon_t* self) {
    char* db_path = g_build_filename(g_get_home_dir(), ".kraken", "events.db", NULL);
    char* dir_path = g_path_get_dirname(db_path);
    
    g_mkdir_with_parents(dir_path, 0755);
    
    int rc = sqlite3_open(db_path, &self->db);
    if (rc != SQLITE_OK) {
        g_printerr("Kraken Daemon: Can't open database: %s\n", sqlite3_errmsg(self->db));
        g_free(db_path);
        g_free(dir_path);
        return FALSE;
    }
    
    const char* sql = "CREATE TABLE IF NOT EXISTS events ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "path TEXT,"
                      "timestamp INTEGER,"
                      "mime_type TEXT,"
                      "actor TEXT,"
                      "confidence REAL,"
                      "toon_payload TEXT,"
                      "snapshot_path TEXT);"
                      "CREATE TABLE IF NOT EXISTS embeddings ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "path TEXT,"
                      "vector BLOB);"
                      "CREATE TABLE IF NOT EXISTS tags ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "name TEXT UNIQUE);"
                      "CREATE TABLE IF NOT EXISTS file_tags ("
                      "path TEXT,"
                      "tag_id INTEGER,"
                      "FOREIGN KEY(tag_id) REFERENCES tags(id),"
                      "PRIMARY KEY(path, tag_id));";
                      
    char* err_msg = NULL;
    rc = sqlite3_exec(self->db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        g_printerr("Kraken Daemon: SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        g_free(db_path);
        g_free(dir_path);
        return FALSE;
    }
    
    g_print("Kraken Daemon: Database initialized at %s\n", db_path);
    g_free(db_path);
    g_free(dir_path);
    return TRUE;
}

kraken_daemon_t* kraken_daemon_new(void) {
    kraken_daemon_t* self = g_new0(kraken_daemon_t, 1);
    self->main_loop = g_main_loop_new(NULL, FALSE);
    self->event_queue = NULL;
    
    if (!kraken_daemon_init_db(self)) {
        g_free(self);
        return NULL;
    }
    
    if (!kraken_daemon_setup_monitoring(self)) {
        /* Continue even if monitoring fails for now? No, better fail early in MVP */
        kraken_daemon_free(self);
        return NULL;
    }
    
    self->llm = kraken_llm_new(NULL);
    
    return self;
}

void kraken_daemon_run(kraken_daemon_t* self) {
    g_print("Kraken Daemon: Submerging into the file system abysm...\n");
    g_main_loop_run(self->main_loop);
}

void kraken_daemon_stop(kraken_daemon_t* self) {
    g_print("Kraken Daemon: Surfacing...\n");
    if (g_main_loop_is_running(self->main_loop)) {
        g_main_loop_quit(self->main_loop);
    }
}

void kraken_daemon_free(kraken_daemon_t* self) {
    if (self->main_loop) {
        g_main_loop_unref(self->main_loop);
    }
    if (self->db) {
        sqlite3_close(self->db);
    }
    if (self->monitor) {
        g_file_monitor_cancel(self->monitor);
        g_object_unref(self->monitor);
    }
    if (self->llm) {
        kraken_llm_free(self->llm);
    }
    /* Free queue and other members here */
    g_free(self);
}


