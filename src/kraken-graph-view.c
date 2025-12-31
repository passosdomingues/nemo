#include <config.h>
#include "kraken-graph-view.h"
#include "kraken-window-slot.h"
#include <libkraken-private/kraken-file.h>
#include <libkraken-private/kraken-directory.h>
#include <math.h>

#define REPULSION_CONSTANT 5000.0
#define ATTRACTION_CONSTANT 0.5
#define DAMPING 0.85
#define TIME_STEP 0.1
#define MIN_DISTANCE 50.0
#define NODE_RADIUS 20.0

struct _KrakenGraphViewDetails {
    GtkWidget *drawing_area;
    GList *nodes;          // List of GraphNode*
    GList *edges;          // List of GraphEdge*
    
    // Interaction state
    GraphNode *dragging_node;
    double drag_start_x, drag_start_y;
    double zoom_level;
    double pan_x, pan_y;
    
    // Animation
    guint layout_timeout_id;
    gboolean layout_running;
};

G_DEFINE_TYPE (KrakenGraphView, kraken_graph_view, KRAKEN_TYPE_VIEW);

static void free_graph_node (GraphNode *node) {
    if (node->file) {
        kraken_file_unref (node->file);
    }
    g_free (node);
}

static void free_graph_edge (GraphEdge *edge) {
    g_free (edge);
}

static void kraken_graph_view_finalize (GObject *object) {
    KrakenGraphView *view = KRAKEN_GRAPH_VIEW (object);
    
    if (view->details->layout_timeout_id) {
        g_source_remove (view->details->layout_timeout_id);
    }
    
    g_list_free_full (view->details->nodes, (GDestroyNotify) free_graph_node);
    g_list_free_full (view->details->edges, (GDestroyNotify) free_graph_edge);
    
    g_free (view->details);
    
    G_OBJECT_CLASS (kraken_graph_view_parent_class)->finalize (object);
}

// Fruchterman-Reingold force-directed layout
static void apply_forces (KrakenGraphView *view) {
    GList *l1, *l2;
    
    // Reset forces
    for (l1 = view->details->nodes; l1; l1 = l1->next) {
        GraphNode *node = l1->data;
        node->vx = 0;
        node->vy = 0;
    }
    
    // Repulsive forces between all nodes
    for (l1 = view->details->nodes; l1; l1 = l1->next) {
        GraphNode *n1 = l1->data;
        for (l2 = l1->next; l2; l2 = l2->next) {
            GraphNode *n2 = l2->data;
            
            double dx = n2->x - n1->x;
            double dy = n2->y - n1->y;
            double dist = sqrt(dx*dx + dy*dy);
            
            if (dist < MIN_DISTANCE) dist = MIN_DISTANCE;
            
            double force = REPULSION_CONSTANT / (dist * dist);
            double fx = (dx / dist) * force;
            double fy = (dy / dist) * force;
            
            n1->vx -= fx;
            n1->vy -= fy;
            n2->vx += fx;
            n2->vy += fy;
        }
    }
    
    // Attractive forces along edges
    for (GList *l = view->details->edges; l; l = l->next) {
        GraphEdge *edge = l->data;
        
        double dx = edge->target->x - edge->source->x;
        double dy = edge->target->y - edge->source->y;
        double dist = sqrt(dx*dx + dy*dy);
        
        if (dist < 1.0) dist = 1.0;
        
        double force = ATTRACTION_CONSTANT * edge->weight * dist;
        double fx = (dx / dist) * force;
        double fy = (dy / dist) * force;
        
        edge->source->vx += fx;
        edge->source->vy += fy;
        edge->target->vx -= fx;
        edge->target->vy -= fy;
    }
    
    // Update positions with damping
    for (l1 = view->details->nodes; l1; l1 = l1->next) {
        GraphNode *node = l1->data;
        
        if (node == view->details->dragging_node) continue;
        
        node->x += node->vx * TIME_STEP;
        node->y += node->vy * TIME_STEP;
        
        node->vx *= DAMPING;
        node->vy *= DAMPING;
    }
}

static gboolean layout_tick (gpointer user_data) {
    KrakenGraphView *view = KRAKEN_GRAPH_VIEW (user_data);
    
    apply_forces (view);
    gtk_widget_queue_draw (view->details->drawing_area);
    
    return G_SOURCE_CONTINUE;
}

static gboolean on_draw (GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    KrakenGraphView *view = KRAKEN_GRAPH_VIEW (user_data);
    GtkAllocation allocation;
    gtk_widget_get_allocation (widget, &allocation);
    
    // Clear background
    cairo_set_source_rgb (cr, 0.95, 0.95, 0.95);
    cairo_paint (cr);
    
    // Apply transformations
    cairo_translate (cr, allocation.width / 2.0 + view->details->pan_x,
                         allocation.height / 2.0 + view->details->pan_y);
    cairo_scale (cr, view->details->zoom_level, view->details->zoom_level);
    
    // Draw edges
    for (GList *l = view->details->edges; l; l = l->next) {
        GraphEdge *edge = l->data;
        
        double alpha = edge->weight * 0.5;
        switch (edge->type) {
            case EDGE_TYPE_TAG_SIMILARITY:
                cairo_set_source_rgba (cr, 0.2, 0.6, 0.9, alpha);
                break;
            case EDGE_TYPE_SEMANTIC_SIMILARITY:
                cairo_set_source_rgba (cr, 0.9, 0.4, 0.2, alpha);
                break;
            case EDGE_TYPE_TEMPORAL_PROXIMITY:
                cairo_set_source_rgba (cr, 0.4, 0.8, 0.4, alpha);
                break;
        }
        
        cairo_set_line_width (cr, edge->weight * 2.0);
        cairo_move_to (cr, edge->source->x, edge->source->y);
        cairo_line_to (cr, edge->target->x, edge->target->y);
        cairo_stroke (cr);
    }
    
    // Draw nodes
    for (GList *l = view->details->nodes; l; l = l->next) {
        GraphNode *node = l->data;
        
        // Node circle
        if (node->selected) {
            cairo_set_source_rgb (cr, 0.2, 0.6, 0.9);
        } else if (node->hovered) {
            cairo_set_source_rgb (cr, 0.5, 0.7, 1.0);
        } else {
            cairo_set_source_rgb (cr, 0.3, 0.3, 0.3);
        }
        
        cairo_arc (cr, node->x, node->y, NODE_RADIUS, 0, 2 * M_PI);
        cairo_fill (cr);
        
        // Node label (on hover)
        if (node->hovered || node->selected) {
            char *name = kraken_file_get_display_name (node->file);
            cairo_set_source_rgb (cr, 0, 0, 0);
            cairo_select_font_face (cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
            cairo_set_font_size (cr, 12);
            cairo_move_to (cr, node->x + NODE_RADIUS + 5, node->y + 5);
            cairo_show_text (cr, name);
            g_free (name);
        }
    }
    
    return FALSE;
}

static GraphNode *find_node_at (KrakenGraphView *view, double x, double y) {
    GtkAllocation allocation;
    gtk_widget_get_allocation (view->details->drawing_area, &allocation);
    
    // Transform screen coordinates to graph coordinates
    x = (x - allocation.width / 2.0 - view->details->pan_x) / view->details->zoom_level;
    y = (y - allocation.height / 2.0 - view->details->pan_y) / view->details->zoom_level;
    
    for (GList *l = view->details->nodes; l; l = l->next) {
        GraphNode *node = l->data;
        double dx = x - node->x;
        double dy = y - node->y;
        if (sqrt(dx*dx + dy*dy) <= NODE_RADIUS) {
            return node;
        }
    }
    return NULL;
}

static gboolean on_button_press (GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
    KrakenGraphView *view = KRAKEN_GRAPH_VIEW (user_data);
    
    GraphNode *node = find_node_at (view, event->x, event->y);
    if (node) {
        view->details->dragging_node = node;
        view->details->drag_start_x = event->x;
        view->details->drag_start_y = event->y;
        node->selected = TRUE;
        gtk_widget_queue_draw (widget);
    }
    
    return TRUE;
}

static gboolean on_button_release (GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
    KrakenGraphView *view = KRAKEN_GRAPH_VIEW (user_data);
    
    if (view->details->dragging_node) {
        view->details->dragging_node->selected = FALSE;
        view->details->dragging_node = NULL;
        gtk_widget_queue_draw (widget);
    }
    
    return TRUE;
}

static gboolean on_motion_notify (GtkWidget *widget, GdkEventMotion *event, gpointer user_data) {
    KrakenGraphView *view = KRAKEN_GRAPH_VIEW (user_data);
    
    if (view->details->dragging_node) {
        GtkAllocation allocation;
        gtk_widget_get_allocation (widget, &allocation);
        
        double dx = (event->x - view->details->drag_start_x) / view->details->zoom_level;
        double dy = (event->y - view->details->drag_start_y) / view->details->zoom_level;
        
        view->details->dragging_node->x += dx;
        view->details->dragging_node->y += dy;
        
        view->details->drag_start_x = event->x;
        view->details->drag_start_y = event->y;
        
        gtk_widget_queue_draw (widget);
    } else {
        // Update hover state
        GraphNode *hovered = find_node_at (view, event->x, event->y);
        gboolean changed = FALSE;
        
        for (GList *l = view->details->nodes; l; l = l->next) {
            GraphNode *node = l->data;
            gboolean was_hovered = node->hovered;
            node->hovered = (node == hovered);
            if (was_hovered != node->hovered) changed = TRUE;
        }
        
        if (changed) gtk_widget_queue_draw (widget);
    }
    
    return TRUE;
}

static void kraken_graph_view_init (KrakenGraphView *view) {
    view->details = g_new0 (KrakenGraphViewDetails, 1);
    view->details->zoom_level = 1.0;
    view->details->pan_x = 0;
    view->details->pan_y = 0;
    
    view->details->drawing_area = gtk_drawing_area_new ();
    gtk_widget_set_size_request (view->details->drawing_area, 800, 600);
    gtk_widget_add_events (view->details->drawing_area,
                          GDK_BUTTON_PRESS_MASK |
                          GDK_BUTTON_RELEASE_MASK |
                          GDK_POINTER_MOTION_MASK);
    
    g_signal_connect (view->details->drawing_area, "draw", G_CALLBACK (on_draw), view);
    g_signal_connect (view->details->drawing_area, "button-press-event", G_CALLBACK (on_button_press), view);
    g_signal_connect (view->details->drawing_area, "button-release-event", G_CALLBACK (on_button_release), view);
    g_signal_connect (view->details->drawing_area, "motion-notify-event", G_CALLBACK (on_motion_notify), view);
    
    gtk_container_add (GTK_CONTAINER (view), view->details->drawing_area);
    gtk_widget_show_all (GTK_WIDGET (view));
    
    // Start layout animation
    view->details->layout_running = TRUE;
    view->details->layout_timeout_id = g_timeout_add (16, layout_tick, view); // ~60 FPS
}

static void kraken_graph_view_class_init (KrakenGraphViewClass *klass) {
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->finalize = kraken_graph_view_finalize;
}

KrakenView *kraken_graph_view_new (KrakenWindowSlot *slot) {
    return g_object_new (KRAKEN_TYPE_GRAPH_VIEW, "window-slot", slot, NULL);
}
