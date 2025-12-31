#include <config.h>
#include "kraken-tag-directory.h"
#include "kraken-tag-manager.h"
#include "kraken-directory-private.h"
#include "kraken-file.h"
#include "kraken-file-private.h"
#include "kraken-file-utilities.h"
#include <gio/gio.h>
#include <string.h>

struct KrakenTagDirectoryDetails {
	GList *files;
	gboolean loading;
	char *tag_name;
};

G_DEFINE_TYPE (KrakenTagDirectory, kraken_tag_directory, KRAKEN_TYPE_DIRECTORY);

static void kraken_tag_directory_finalize (GObject *object) {
	KrakenTagDirectory *self = KRAKEN_TAG_DIRECTORY (object);
	g_list_free_full (self->details->files, g_object_unref);
	g_free (self->details->tag_name);
	g_free (self->details);
	G_OBJECT_CLASS (kraken_tag_directory_parent_class)->finalize (object);
}

static void on_tags_received (GObject *source, GAsyncResult *res, gpointer user_data) {
    KrakenTagDirectory *self = KRAKEN_TAG_DIRECTORY (user_data);
    GError *error = NULL;
    GList *tags = kraken_tag_manager_get_tags_finish (kraken_tag_manager_get(), res, &error);
    
    if (error) {
        g_warning ("KrakenTagDirectory: Failed to get tags: %s", error->message);
        g_error_free (error);
    } else {
        GList *added_files = NULL;
        for (GList *l = tags; l != NULL; l = l->next) {
            char *tag_name = l->data;
            char *uri = g_strdup_printf ("kraken://tags/%s", tag_name);
            KrakenFile *file = kraken_file_get_by_uri (uri);
            /* Mark as folder in metadata if possible, but KrakenFile usually does this via URI scheme */
            self->details->files = g_list_append (self->details->files, file);
            added_files = g_list_append (added_files, file);
            g_free (uri);
            g_free (tag_name);
        }
        g_list_free (tags);
        
        kraken_directory_emit_files_added (KRAKEN_DIRECTORY (self), added_files);
        g_list_free (added_files);
    }
    
    self->details->loading = FALSE;
    kraken_directory_emit_done_loading (KRAKEN_DIRECTORY (self));
    g_object_unref (self);
}

static void on_files_received (GObject *source, GAsyncResult *res, gpointer user_data) {
    KrakenTagDirectory *self = KRAKEN_TAG_DIRECTORY (user_data);
    GError *error = NULL;
    GList *results = kraken_tag_manager_get_files_by_tag_finish (kraken_tag_manager_get(), res, &error);
    
    if (error) {
        g_warning ("KrakenTagDirectory: Failed to get files: %s", error->message);
        g_error_free (error);
    } else {
        GList *added_files = NULL;
        for (GList *l = results; l != NULL; l = l->next) {
            SearchResult *sr = l->data;
            KrakenFile *file = kraken_file_get_by_uri (g_filename_to_uri(sr->path, NULL, NULL));
            self->details->files = g_list_append (self->details->files, file);
            added_files = g_list_append (added_files, file);
            g_free (sr->path);
            g_free (sr);
        }
        g_list_free (results);
        
        kraken_directory_emit_files_added (KRAKEN_DIRECTORY (self), added_files);
        g_list_free (added_files);
    }
    
    self->details->loading = FALSE;
    kraken_directory_emit_done_loading (KRAKEN_DIRECTORY (self));
    g_object_unref (self);
}

static void kraken_tag_directory_force_reload (KrakenDirectory *directory) {
    KrakenTagDirectory *self = KRAKEN_TAG_DIRECTORY (directory);
    char *uri = kraken_directory_get_uri (directory);
    
    if (self->details->loading) return;
    self->details->loading = TRUE;
    
    /* Clear existing files */
    g_list_free_full (self->details->files, g_object_unref);
    self->details->files = NULL;
    
    g_object_ref (self);
    if (strcmp (uri, "kraken://tags/") == 0 || strcmp (uri, "kraken://tags") == 0) {
        kraken_tag_manager_get_tags_async (kraken_tag_manager_get(), on_tags_received, self);
    } else if (g_str_has_prefix (uri, "kraken://tags/")) {
        char *tag = uri + strlen("kraken://tags/");
        kraken_tag_manager_get_files_by_tag_async (kraken_tag_manager_get(), tag, on_files_received, self);
    }
    g_free (uri);
}

static gboolean kraken_tag_directory_are_all_files_seen (KrakenDirectory *directory) {
    return !KRAKEN_TAG_DIRECTORY (directory)->details->loading;
}

static GList *kraken_tag_directory_get_file_list (KrakenDirectory *directory) {
    return kraken_file_list_copy (KRAKEN_TAG_DIRECTORY (directory)->details->files);
}

static void kraken_tag_directory_init (KrakenTagDirectory *self) {
	self->details = g_new0 (KrakenTagDirectoryDetails, 1);
}

static void kraken_tag_directory_class_init (KrakenTagDirectoryClass *klass) {
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	KrakenDirectoryClass *directory_class = KRAKEN_DIRECTORY_CLASS (klass);

	object_class->finalize = kraken_tag_directory_finalize;
	directory_class->force_reload = kraken_tag_directory_force_reload;
	directory_class->are_all_files_seen = kraken_tag_directory_are_all_files_seen;
	directory_class->get_file_list = kraken_tag_directory_get_file_list;
	directory_class->is_editable = (gboolean (*) (KrakenDirectory *)) 0; /* Not editable */
}
