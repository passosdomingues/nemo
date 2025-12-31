#include <config.h>
#include "kraken-timeline-view.h"
#include <time.h>
#include <math.h>

// Forward declaration from daemon
typedef struct {
    char *path;
    float score;
} SearchResult;

struct _KrakenTimelineViewDetails {
    GtkWidget *scrolled_window;
    GtkWidget *timeline_box;
    GList *clusters;
    ClusterGranularity granularity;
};

G_DEFINE_TYPE (KrakenTimelineView, kraken_timeline_view, GTK_TYPE_BOX);

static void free_timeline_cluster (TimelineCluster *cluster) {
    g_list_free_full (cluster->files, g_free);
    g_free (cluster->context_summary);
    g_free (cluster);
}

static void kraken_timeline_view_finalize (GObject *object) {
    KrakenTimelineView *view = KRAKEN_TIMELINE_VIEW (object);
    g_list_free_full (view->details->clusters, (GDestroyNotify) free_timeline_cluster);
    g_free (view->details);
    G_OBJECT_CLASS (kraken_timeline_view_parent_class)->finalize (object);
}

static time_t normalize_to_day (time_t t) {
    struct tm *tm_info = localtime (&t);
    tm_info->tm_hour = 0;
    tm_info->tm_min = 0;
    tm_info->tm_sec = 0;
    return mktime (tm_info);
}

static time_t normalize_to_week (time_t t) {
    struct tm *tm_info = localtime (&t);
    tm_info->tm_mday -= tm_info->tm_wday; // Start of week
    tm_info->tm_hour = 0;
    tm_info->tm_min = 0;
    tm_info->tm_sec = 0;
    return mktime (tm_info);
}

static time_t normalize_to_month (time_t t) {
    struct tm *tm_info = localtime (&t);
    tm_info->tm_mday = 1; // First day of month
    tm_info->tm_hour = 0;
    tm_info->tm_min = 0;
    tm_info->tm_sec = 0;
    return mktime (tm_info);
}

static GList *cluster_by_time (GList *results, ClusterGranularity granularity) {
    GHashTable *cluster_map = g_hash_table_new (g_direct_hash, g_direct_equal);
    GList *clusters = NULL;
    
    for (GList *l = results; l; l = l->next) {
        SearchResult *result = l->data;
        
        // Get file modification time (simplified - would use actual file stat)
        time_t mtime = time (NULL); // Placeholder
        time_t normalized;
        
        switch (granularity) {
            case CLUSTER_DAY:
                normalized = normalize_to_day (mtime);
                break;
            case CLUSTER_WEEK:
                normalized = normalize_to_week (mtime);
                break;
            case CLUSTER_MONTH:
                normalized = normalize_to_month (mtime);
                break;
        }
        
        TimelineCluster *cluster = g_hash_table_lookup (cluster_map, GINT_TO_POINTER (normalized));
        if (!cluster) {
            cluster = g_new0 (TimelineCluster, 1);
            cluster->start_time = normalized;
            cluster->end_time = normalized + (granularity == CLUSTER_DAY ? 86400 : 
                                             granularity == CLUSTER_WEEK ? 604800 : 2592000);
            cluster->files = NULL;
            cluster->context_summary = g_strdup ("Loading context..."); // Placeholder for LLM
            cluster->expanded = FALSE;
            
            g_hash_table_insert (cluster_map, GINT_TO_POINTER (normalized), cluster);
            clusters = g_list_append (clusters, cluster);
        }
        
        cluster->files = g_list_append (cluster->files, g_strdup (result->path));
    }
    
    g_hash_table_destroy (cluster_map);
    return clusters;
}

static void on_cluster_expand_clicked (GtkButton *button, gpointer user_data) {
    TimelineCluster *cluster = user_data;
    cluster->expanded = !cluster->expanded;
    
    // TODO: Update UI to show/hide file list
    gtk_button_set_label (button, cluster->expanded ? "▼ Collapse" : "▶ Expand");
}

static GtkWidget *create_cluster_card (TimelineCluster *cluster) {
    GtkWidget *frame = gtk_frame_new (NULL);
    GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width (GTK_CONTAINER (vbox), 10);
    
    // Header with date range
    char date_str[256];
    struct tm *tm_info = localtime (&cluster->start_time);
    strftime (date_str, sizeof (date_str), "%B %d, %Y", tm_info);
    
    GtkWidget *header = gtk_label_new (NULL);
    char *markup = g_strdup_printf ("<b>%s</b> — %d files", date_str, 
                                    g_list_length (cluster->files));
    gtk_label_set_markup (GTK_LABEL (header), markup);
    g_free (markup);
    gtk_box_pack_start (GTK_BOX (vbox), header, FALSE, FALSE, 0);
    
    // Context summary
    GtkWidget *summary = gtk_label_new (cluster->context_summary);
    gtk_label_set_line_wrap (GTK_LABEL (summary), TRUE);
    gtk_widget_set_halign (summary, GTK_ALIGN_START);
    gtk_box_pack_start (GTK_BOX (vbox), summary, FALSE, FALSE, 0);
    
    // Expand button
    GtkWidget *expand_btn = gtk_button_new_with_label ("▶ Expand");
    g_signal_connect (expand_btn, "clicked", G_CALLBACK (on_cluster_expand_clicked), cluster);
    gtk_box_pack_start (GTK_BOX (vbox), expand_btn, FALSE, FALSE, 0);
    
    gtk_container_add (GTK_CONTAINER (frame), vbox);
    return frame;
}

static void kraken_timeline_view_init (KrakenTimelineView *view) {
    view->details = g_new0 (KrakenTimelineViewDetails, 1);
    view->details->granularity = CLUSTER_DAY;
    
    gtk_orientable_set_orientation (GTK_ORIENTABLE (view), GTK_ORIENTATION_VERTICAL);
    
    view->details->scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (view->details->scrolled_window),
                                    GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    
    view->details->timeline_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width (GTK_CONTAINER (view->details->timeline_box), 10);
    
    gtk_container_add (GTK_CONTAINER (view->details->scrolled_window), view->details->timeline_box);
    gtk_box_pack_start (GTK_BOX (view), view->details->scrolled_window, TRUE, TRUE, 0);
    
    gtk_widget_show_all (GTK_WIDGET (view));
}

static void kraken_timeline_view_class_init (KrakenTimelineViewClass *klass) {
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->finalize = kraken_timeline_view_finalize;
}

GtkWidget *kraken_timeline_view_new (void) {
    return g_object_new (KRAKEN_TYPE_TIMELINE_VIEW, NULL);
}

void kraken_timeline_view_set_results (KrakenTimelineView *view, GList *results) {
    // Clear existing clusters
    g_list_free_full (view->details->clusters, (GDestroyNotify) free_timeline_cluster);
    view->details->clusters = NULL;
    
    // Clear timeline box
    gtk_container_foreach (GTK_CONTAINER (view->details->timeline_box),
                          (GtkCallback) gtk_widget_destroy, NULL);
    
    // Create new clusters
    view->details->clusters = cluster_by_time (results, view->details->granularity);
    
    // Render cluster cards
    for (GList *l = view->details->clusters; l; l = l->next) {
        TimelineCluster *cluster = l->data;
        GtkWidget *card = create_cluster_card (cluster);
        gtk_box_pack_start (GTK_BOX (view->details->timeline_box), card, FALSE, FALSE, 0);
    }
    
    gtk_widget_show_all (view->details->timeline_box);
}
