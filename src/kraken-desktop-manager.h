/* kraken-desktop-manager.h */

#ifndef _KRAKEN_DESKTOP_MANAGER_H
#define _KRAKEN_DESKTOP_MANAGER_H

#include <glib-object.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h>

#include "kraken-window.h"

G_BEGIN_DECLS

#define KRAKEN_TYPE_DESKTOP_MANAGER kraken_desktop_manager_get_type ()

G_DECLARE_FINAL_TYPE (KrakenDesktopManager, kraken_desktop_manager, KRAKEN, DESKTOP_MANAGER, GObject)

typedef enum {
    DESKTOP_ARRANGE_VERTICAL,
    DESKTOP_ARRANGE_HORIZONTAL
} KrakenDesktopLayoutDirection;

KrakenDesktopManager* kraken_desktop_manager_get (void);

gboolean kraken_desktop_manager_has_desktop_windows (KrakenDesktopManager *manager);
gboolean kraken_desktop_manager_get_monitor_is_active (KrakenDesktopManager *manager,
                                                                   gint  monitor);
gboolean kraken_desktop_manager_get_monitor_is_primary (KrakenDesktopManager *manager,
                                                                   gint  monitor);

gboolean kraken_desktop_manager_get_primary_only (KrakenDesktopManager *manager);
void     kraken_desktop_manager_get_window_rect_for_monitor (KrakenDesktopManager *manager,
                                                           gint                monitor,
                                                           GdkRectangle       *rect);
gboolean kraken_desktop_manager_has_good_workarea_info (KrakenDesktopManager *manager);

void     kraken_desktop_manager_get_margins             (KrakenDesktopManager *manager,
                                                       gint                monitor,
                                                       gint               *left,
                                                       gint               *right,
                                                       gint               *top,
                                                       gint               *bottom);

GtkWindow *kraken_desktop_manager_get_window_for_monitor  (KrakenDesktopManager *manager,
                                                         gint                monitor);
void kraken_desktop_manager_get_overlay_info              (KrakenDesktopManager *manager,
                                                         gint                monitor,
                                                         GtkActionGroup    **action_group,
                                                         gint               *h_adjust,
                                                         gint               *v_adjust);
void     kraken_desktop_manager_show_desktop_overlay    (KrakenDesktopManager *manager,
                                                       gint                initial_monitor);
gboolean kraken_desktop_manager_get_is_cinnamon         (KrakenDesktopManager *manager);

G_END_DECLS

#endif /* _KRAKEN_DESKTOP_MANAGER_H */
