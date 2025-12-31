#ifndef _KRAKEN_DESKTOP_PREFERENCES_H_
#define _KRAKEN_DESKTOP_PREFERENCES_H_

#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define KRAKEN_TYPE_DESKTOP_PREFERENCES (kraken_desktop_preferences_get_type ())

G_DECLARE_FINAL_TYPE (KrakenDesktopPreferences, kraken_desktop_preferences, KRAKEN, DESKTOP_PREFERENCES, GtkBin)

KrakenDesktopPreferences *kraken_desktop_preferences_new (void);

G_END_DECLS

#endif /* _KRAKEN_DESKTOP_PREFERENCES_H_ */