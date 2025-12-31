/* kraken-pathbar.h
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street - Suite 500,
 * Boston, MA 02110-1335, USA.
 *
 * 
 */

#ifndef KRAKEN_PATHBAR_H
#define KRAKEN_PATHBAR_H

#include <gtk/gtk.h>
#include <gio/gio.h>

typedef struct _KrakenPathBar      KrakenPathBar;
typedef struct _KrakenPathBarClass KrakenPathBarClass;
typedef struct _KrakenPathBarDetails KrakenPathBarDetails;

#define KRAKEN_TYPE_PATH_BAR                 (kraken_path_bar_get_type ())
#define KRAKEN_PATH_BAR(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_PATH_BAR, KrakenPathBar))
#define KRAKEN_PATH_BAR_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_PATH_BAR, KrakenPathBarClass))
#define KRAKEN_IS_PATH_BAR(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_PATH_BAR))
#define KRAKEN_IS_PATH_BAR_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_PATH_BAR))
#define KRAKEN_PATH_BAR_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_PATH_BAR, KrakenPathBarClass))

struct _KrakenPathBar
{
	GtkContainer parent;
	
	KrakenPathBarDetails *priv;
};

struct _KrakenPathBarClass
{
	GtkContainerClass parent_class;

  	void (* path_clicked)   (KrakenPathBar  *path_bar,
				 GFile             *location);
  	void (* path_set)       (KrakenPathBar  *path_bar,
				 GFile             *location);
};

GType    kraken_path_bar_get_type (void) G_GNUC_CONST;

gboolean kraken_path_bar_set_path    (KrakenPathBar *path_bar, GFile *file);
GFile *  kraken_path_bar_get_path_for_button (KrakenPathBar *path_bar,
						GtkWidget       *button);
void     kraken_path_bar_clear_buttons (KrakenPathBar *path_bar);

#endif /* KRAKEN_PATHBAR_H */
