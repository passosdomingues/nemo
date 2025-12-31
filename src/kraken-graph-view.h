#ifndef KRAKEN_GRAPH_VIEW_H
#define KRAKEN_GRAPH_VIEW_H

#include "kraken-view.h"
#include <gtk/gtk.h>

#define KRAKEN_TYPE_GRAPH_VIEW kraken_graph_view_get_type()
#define KRAKEN_GRAPH_VIEW(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_GRAPH_VIEW, KrakenGraphView))
#define KRAKEN_GRAPH_VIEW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_GRAPH_VIEW, KrakenGraphViewClass))
#define KRAKEN_IS_GRAPH_VIEW(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_GRAPH_VIEW))

typedef struct _KrakenGraphView KrakenGraphView;
typedef struct _KrakenGraphViewClass KrakenGraphViewClass;
typedef struct _KrakenGraphViewDetails KrakenGraphViewDetails;

typedef enum {
    EDGE_TYPE_TAG_SIMILARITY,
    EDGE_TYPE_SEMANTIC_SIMILARITY,
    EDGE_TYPE_TEMPORAL_PROXIMITY
} EdgeType;

typedef struct {
    KrakenFile *file;
    double x, y;           // Position
    double vx, vy;         // Velocity for force simulation
    double mass;
    gboolean selected;
    gboolean hovered;
} GraphNode;

typedef struct {
    GraphNode *source;
    GraphNode *target;
    double weight;         // 0.0 to 1.0
    EdgeType type;
} GraphEdge;

struct _KrakenGraphView {
    KrakenView parent;
    KrakenGraphViewDetails *details;
};

struct _KrakenGraphViewClass {
    KrakenViewClass parent_class;
};

GType kraken_graph_view_get_type (void);
KrakenView *kraken_graph_view_new (KrakenWindowSlot *slot);

#endif /* KRAKEN_GRAPH_VIEW_H */
