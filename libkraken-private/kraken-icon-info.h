#ifndef KRAKEN_ICON_INFO_H
#define KRAKEN_ICON_INFO_H

#include <glib-object.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gdk/gdk.h>
#include <gio/gio.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

/* Names for Kraken's different zoom levels, from tiniest items to largest items */
typedef enum {
    KRAKEN_ZOOM_LEVEL_NULL = -1,
	KRAKEN_ZOOM_LEVEL_SMALLEST = 0,
	KRAKEN_ZOOM_LEVEL_SMALLER,
	KRAKEN_ZOOM_LEVEL_SMALL,
	KRAKEN_ZOOM_LEVEL_STANDARD,
	KRAKEN_ZOOM_LEVEL_LARGE,
	KRAKEN_ZOOM_LEVEL_LARGER,
	KRAKEN_ZOOM_LEVEL_LARGEST
} KrakenZoomLevel;

#define KRAKEN_ZOOM_LEVEL_N_ENTRIES (KRAKEN_ZOOM_LEVEL_LARGEST + 1)

/* Nominal icon sizes for each Kraken zoom level.
 * This scheme assumes that icons are designed to
 * fit in a square space, though each image needn't
 * be square. Since individual icons can be stretched,
 * each icon is not constrained to this nominal size.
 */

#define KRAKEN_COMPACT_FORCED_ICON_SIZE 16

#define KRAKEN_LIST_ICON_SIZE_SMALLEST 16
#define KRAKEN_LIST_ICON_SIZE_SMALLER  16
#define KRAKEN_LIST_ICON_SIZE_SMALL    24
#define KRAKEN_LIST_ICON_SIZE_STANDARD 32
#define KRAKEN_LIST_ICON_SIZE_LARGE    48
#define KRAKEN_LIST_ICON_SIZE_LARGER   72
#define KRAKEN_LIST_ICON_SIZE_LARGEST  96

#define KRAKEN_ICON_SIZE_SMALLEST 24
#define KRAKEN_ICON_SIZE_SMALLER  32
#define KRAKEN_ICON_SIZE_SMALL    48
#define KRAKEN_ICON_SIZE_STANDARD 64
#define KRAKEN_ICON_SIZE_LARGE    96
#define KRAKEN_ICON_SIZE_LARGER   128
#define KRAKEN_ICON_SIZE_LARGEST  256

#define KRAKEN_DESKTOP_ICON_SIZE_SMALLER 24
#define KRAKEN_DESKTOP_ICON_SIZE_SMALL 32
#define KRAKEN_DESKTOP_ICON_SIZE_STANDARD 48
#define KRAKEN_DESKTOP_ICON_SIZE_LARGE 64
#define KRAKEN_DESKTOP_ICON_SIZE_LARGER 96

#define KRAKEN_DESKTOP_TEXT_WIDTH_SMALLER 64
#define KRAKEN_DESKTOP_TEXT_WIDTH_SMALL 84
#define KRAKEN_DESKTOP_TEXT_WIDTH_STANDARD 110
#define KRAKEN_DESKTOP_TEXT_WIDTH_LARGE 150
#define KRAKEN_DESKTOP_TEXT_WIDTH_LARGER 200

#define KRAKEN_ICON_TEXT_WIDTH_SMALLEST  0
#define KRAKEN_ICON_TEXT_WIDTH_SMALLER   64
#define KRAKEN_ICON_TEXT_WIDTH_SMALL     84
#define KRAKEN_ICON_TEXT_WIDTH_STANDARD  110
#define KRAKEN_ICON_TEXT_WIDTH_LARGE     96
#define KRAKEN_ICON_TEXT_WIDTH_LARGER    128
#define KRAKEN_ICON_TEXT_WIDTH_LARGEST   256

/* Maximum size of an icon that the icon factory will ever produce */
#define KRAKEN_ICON_MAXIMUM_SIZE     320

typedef struct {
    gint ref_count;
    gboolean sole_owner;
    gint64 last_use_time;
    GdkPixbuf *pixbuf;

    char *icon_name;
    gint orig_scale;
} KrakenIconInfo;

KrakenIconInfo *    kraken_icon_info_ref                          (KrakenIconInfo      *icon);
void              kraken_icon_info_unref                        (KrakenIconInfo      *icon);
void              kraken_icon_info_clear                        (KrakenIconInfo     **info);
KrakenIconInfo *    kraken_icon_info_new_for_pixbuf               (GdkPixbuf         *pixbuf,
                                                               int                scale);
KrakenIconInfo *    kraken_icon_info_lookup                       (GIcon             *icon,
                                                               int                size,
                                                               int                scale);
KrakenIconInfo *    kraken_icon_info_lookup_from_name             (const char        *name,
                                                               int                size,
                                                               int                scale);
KrakenIconInfo *    kraken_icon_info_lookup_from_path             (const char        *path,
                                                               int                size,
                                                               int                scale);
gboolean              kraken_icon_info_is_fallback                  (KrakenIconInfo  *icon);
GdkPixbuf *           kraken_icon_info_get_pixbuf                   (KrakenIconInfo  *icon);
GdkPixbuf *           kraken_icon_info_get_pixbuf_nodefault         (KrakenIconInfo  *icon);
GdkPixbuf *           kraken_icon_info_get_pixbuf_nodefault_at_size (KrakenIconInfo  *icon,
								       gsize              forced_size);
GdkPixbuf *           kraken_icon_info_get_pixbuf_at_size           (KrakenIconInfo  *icon,
								       gsize              forced_size);
GdkPixbuf *           kraken_icon_info_get_desktop_pixbuf_at_size (KrakenIconInfo  *icon,
                                                                 gsize          max_height,
                                                                 gsize          max_width);
const char *          kraken_icon_info_get_used_name                (KrakenIconInfo  *icon);

void                  kraken_icon_info_clear_caches                 (void);

/* Relationship between zoom levels and icons sizes. */
guint kraken_get_icon_size_for_zoom_level          (KrakenZoomLevel  zoom_level);
guint kraken_get_icon_text_width_for_zoom_level    (KrakenZoomLevel  zoom_level);

guint kraken_get_list_icon_size_for_zoom_level     (KrakenZoomLevel  zoom_level);

guint kraken_get_desktop_icon_size_for_zoom_level  (KrakenZoomLevel  zoom_level);
guint kraken_get_desktop_text_width_for_zoom_level (KrakenZoomLevel  zoom_level);

gint  kraken_get_icon_size_for_stock_size          (GtkIconSize        size);
guint kraken_icon_get_emblem_size_for_icon_size    (guint              size);

GIcon * kraken_user_special_directory_get_gicon (GUserDirectory directory);


G_END_DECLS

#endif /* KRAKEN_ICON_INFO_H */

