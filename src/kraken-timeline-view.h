#ifndef KRAKEN_TIMELINE_VIEW_H
#define KRAKEN_TIMELINE_VIEW_H

#include <gtk/gtk.h>
#include <glib.h>

#define KRAKEN_TYPE_TIMELINE_VIEW kraken_timeline_view_get_type()
#define KRAKEN_TIMELINE_VIEW(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_TIMELINE_VIEW, KrakenTimelineView))

typedef struct _KrakenTimelineView KrakenTimelineView;
typedef struct _KrakenTimelineViewClass KrakenTimelineViewClass;
typedef struct _KrakenTimelineViewDetails KrakenTimelineViewDetails;

typedef enum {
    CLUSTER_DAY,
    CLUSTER_WEEK,
    CLUSTER_MONTH
} ClusterGranularity;

typedef struct {
    time_t start_time;
    time_t end_time;
    GList *files;          // List of file paths
    char *context_summary; // LLM-generated summary
    gboolean expanded;
} TimelineCluster;

struct _KrakenTimelineView {
    GtkBox parent;
    KrakenTimelineViewDetails *details;
};

struct _KrakenTimelineViewClass {
    GtkBoxClass parent_class;
};

GType kraken_timeline_view_get_type (void);
GtkWidget *kraken_timeline_view_new (void);
void kraken_timeline_view_set_results (KrakenTimelineView *view, GList *results);

#endif /* KRAKEN_TIMELINE_VIEW_H */
