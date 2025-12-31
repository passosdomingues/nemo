#ifndef _KRAKEN_DESKTOP_OVERLAY_H_
#define _KRAKEN_DESKTOP_OVERLAY_H_

#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define KRAKEN_TYPE_DESKTOP_OVERLAY (kraken_desktop_overlay_get_type ())

G_DECLARE_FINAL_TYPE (KrakenDesktopOverlay, kraken_desktop_overlay, KRAKEN, DESKTOP_OVERLAY, GObject)

KrakenDesktopOverlay *kraken_desktop_overlay_new (void);
void                kraken_desktop_overlay_show (KrakenDesktopOverlay *overlay,
                                               gint                monitor);
void                kraken_desktop_overlay_update_in_place (KrakenDesktopOverlay *overlay);
G_END_DECLS

#endif /* _KRAKEN_DESKTOP_OVERLAY_H_ */