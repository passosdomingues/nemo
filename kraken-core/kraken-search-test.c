#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include "kraken-daemon.h"

static void on_search_done(GObject* source, GAsyncResult* res, gpointer user_data) {
    GMainLoop* loop = (GMainLoop*)user_data;
    GError* error = NULL;
    GList* results = kraken_daemon_search_finish(NULL, res, &error);
    
    if (error) {
        g_printerr("Search failed: %s\n", error->message);
        g_error_free(error);
    } else {
        printf("\n--- Search Results ---\n");
        if (!results) {
            printf("No relevant files found.\n");
        }
        for (GList* l = results; l != NULL; l = l->next) {
            SearchResult* sr = (SearchResult*)l->data; // This is internal but we know the struct
            printf("[Score: %.4f] %s\n", sr->score, sr->path);
            g_free(sr->path);
            g_free(sr);
        }
        g_list_free(results);
        printf("----------------------\n");
    }
    
    g_main_loop_quit(loop);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <query string>\n", argv[0]);
        return 1;
    }
    
    GMainLoop* loop = g_main_loop_new(NULL, FALSE);
    kraken_daemon_t* daemon = kraken_daemon_new();
    
    if (!daemon) {
        fprintf(stderr, "Failed to initialize Kraken daemon context.\n");
        return 1;
    }
    
    printf("Searching for: '%s'...\n", argv[1]);
    kraken_daemon_search_async(daemon, argv[1], on_search_done, loop);
    
    g_main_loop_run(loop);
    
    kraken_daemon_free(daemon);
    g_main_loop_unref(loop);
    
    return 0;
}
