#include <config.h>
#include <gtk/gtk.h>
#include <sqlite3.h>
#include <libkraken-extension/kraken-property-page-provider.h>
#include "kraken-sidebar-provider.h"

struct _KrakenSidebarProvider {
    GObject parent_instance;
};

static void property_page_provider_iface_init(KrakenPropertyPageProviderIface* iface);

G_DEFINE_TYPE_WITH_CODE(KrakenSidebarProvider, kraken_sidebar_provider, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE(KRAKEN_TYPE_PROPERTY_PAGE_PROVIDER,
                                               property_page_provider_iface_init))

static void kraken_sidebar_provider_init(KrakenSidebarProvider* self) {}

static void kraken_sidebar_provider_class_init(KrakenSidebarProviderClass* klass) {}

static char* get_summary_from_db(const char* file_path) {
    char* db_path = g_build_filename(g_get_home_dir(), ".kraken", "events.db", NULL);
    sqlite3* db;
    char* summary = NULL;
    
    if (sqlite3_open(db_path, &db) == SQLITE_OK) {
        const char* sql = "SELECT json_extract(toon_payload, '$.summary') FROM events WHERE path = ? AND toon_payload LIKE '%summary%' ORDER BY id DESC LIMIT 1;";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, file_path, -1, SQLITE_TRANSIENT);
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                const unsigned char* text = sqlite3_column_text(stmt, 0);
                if (text) summary = g_strdup((const char*)text);
            }
            sqlite3_finalize(stmt);
        }
        sqlite3_close(db);
    }
    
    g_free(db_path);
    return summary;
}
static void save_tag_to_db(const char* file_path, const char* tag) {
    char* db_path = g_build_filename(g_get_home_dir(), ".kraken", "events.db", NULL);
    sqlite3* db;
    
    if (sqlite3_open(db_path, &db) == SQLITE_OK) {
        /* Append tag to toon_payload using json_insert or json_append (sqlite3 >= 3.32) */
        const char* sql = "UPDATE events SET toon_payload = json_insert(toon_payload, '$.tags[#]', ?) "
                          "WHERE path = ? AND id = (SELECT max(id) FROM events WHERE path = ?);";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, tag, -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, file_path, -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 3, file_path, -1, SQLITE_TRANSIENT);
            sqlite3_step(stmt);
            sqlite3_finalize(stmt);
        }
        sqlite3_close(db);
    }
    g_free(db_path);
}

static GList* get_tags_from_db(const char* file_path) {
    char* db_path = g_build_filename(g_get_home_dir(), ".kraken", "events.db", NULL);
    sqlite3* db;
    GList* tags = NULL;
    
    if (sqlite3_open(db_path, &db) == SQLITE_OK) {
        const char* sql = "SELECT value FROM events, json_each(toon_payload, '$.tags') WHERE path = ? ORDER BY id DESC LIMIT 20;";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, file_path, -1, SQLITE_TRANSIENT);
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                tags = g_list_append(tags, g_strdup((const char*)sqlite3_column_text(stmt, 0)));
            }
            sqlite3_finalize(stmt);
        }
        sqlite3_close(db);
    }
    g_free(db_path);
    return tags;
}

static void on_tag_activated(GtkEntry* entry, gpointer user_data) {
    const char* tag = gtk_entry_get_text(entry);
    const char* path = (const char*)user_data;
    if (tag && *tag) {
        save_tag_to_db(path, tag);
        gtk_entry_set_text(entry, "");
        /* In a real app we'd refresh the UI here */
    }
}

static GList* kraken_sidebar_provider_get_pages(KrakenPropertyPageProvider* provider, GList* files) {
    if (g_list_length(files) != 1) return NULL;
    
    KrakenFileInfo* file = KRAKEN_FILE_INFO(files->data);
    char* uri = kraken_file_info_get_uri(file);
    char* path = g_filename_from_uri(uri, NULL, NULL);
    
    char* summary = get_summary_from_db(path);
    if (!summary) summary = g_strdup("Nenhum dado semântico encontrado.");
    GList* tags = get_tags_from_db(path);
    
    /* Create UI */
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 12);
    
    /* Summary Section */
    GtkWidget* label_title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label_title), "<b>Resumo Kraken (IA)</b>");
    gtk_label_set_xalign(GTK_LABEL(label_title), 0.0);
    gtk_box_pack_start(GTK_BOX(vbox), label_title, FALSE, FALSE, 0);
    
    GtkWidget* label_summary = gtk_label_new(summary);
    gtk_label_set_line_wrap(GTK_LABEL(label_summary), TRUE);
    gtk_label_set_xalign(GTK_LABEL(label_summary), 0.0);
    gtk_box_pack_start(GTK_BOX(vbox), label_summary, FALSE, FALSE, 0);

    /* Tagging Section */
    GtkWidget* separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(vbox), separator, FALSE, FALSE, 6);

    GtkWidget* label_tags_title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label_tags_title), "<b>Tags Semânticas</b>");
    gtk_label_set_xalign(GTK_LABEL(label_tags_title), 0.0);
    gtk_box_pack_start(GTK_BOX(vbox), label_tags_title, FALSE, FALSE, 0);

    GtkWidget* flowbox = gtk_flow_box_new();
    gtk_box_pack_start(GTK_BOX(vbox), flowbox, FALSE, FALSE, 0);

    for (GList* l = tags; l != NULL; l = l->next) {
        GtkWidget* tag_label = gtk_label_new((const char*)l->data);
        gtk_widget_set_name(tag_label, "kraken-tag");
        gtk_flow_box_insert(GTK_FLOW_BOX(flowbox), tag_label, -1);
    }

    GtkWidget* tag_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(tag_entry), "Adicionar tag semântica...");
    g_signal_connect(tag_entry, "activate", G_CALLBACK(on_tag_activated), g_strdup(path));
    gtk_box_pack_start(GTK_BOX(vbox), tag_entry, FALSE, FALSE, 0);
    
    gtk_widget_show_all(vbox);
    
    KrakenPropertyPage* page = kraken_property_page_new("kraken-semantic",
                                                       gtk_label_new("Semântica"),
                                                       vbox);
    
    g_free(uri);
    g_free(path);
    g_free(summary);
    
    return g_list_append(NULL, page);
}

static void property_page_provider_iface_init(KrakenPropertyPageProviderIface* iface) {
    iface->get_pages = kraken_sidebar_provider_get_pages;
}

/* Extension module entry points */
void kraken_module_initialize(GTypeModule* module) {
    kraken_sidebar_provider_register_type(module);
}

void kraken_module_shutdown(void) {}

void kraken_module_list_types(const GType** types, int* num_types) {
    static GType type_list[1];
    type_list[0] = KRAKEN_TYPE_SIDEBAR_PROVIDER;
    *types = type_list;
    *num_types = 1;
}

void kraken_sidebar_provider_register_type(GTypeModule* module) {
    kraken_sidebar_provider_get_type();
}
