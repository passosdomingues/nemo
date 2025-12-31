/*
 *  Copyright © 2002 Christophe Fergeau
 *  Copyright © 2003 Marco Pesenti Gritti
 *  Copyright © 2003, 2004 Christian Persch
 *    (ephy-notebook.c)
 *
 *  Copyright © 2008 Free Software Foundation, Inc.
 *    (kraken-notebook.c)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *  $Id: kraken-notebook.h 8210 2008-04-11 20:05:25Z chpe $
 */

#ifndef KRAKEN_NOTEBOOK_H
#define KRAKEN_NOTEBOOK_H

#include <glib.h>
#include <gtk/gtk.h>
#include "kraken-window-slot.h"

G_BEGIN_DECLS

#define KRAKEN_TYPE_NOTEBOOK		(kraken_notebook_get_type ())
#define KRAKEN_NOTEBOOK(o)		(G_TYPE_CHECK_INSTANCE_CAST ((o), KRAKEN_TYPE_NOTEBOOK, KrakenNotebook))
#define KRAKEN_NOTEBOOK_CLASS(k)		(G_TYPE_CHECK_CLASS_CAST((k), KRAKEN_TYPE_NOTEBOOK, KrakenNotebookClass))
#define KRAKEN_IS_NOTEBOOK(o)		(G_TYPE_CHECK_INSTANCE_TYPE ((o), KRAKEN_TYPE_NOTEBOOK))
#define KRAKEN_IS_NOTEBOOK_CLASS(k)	(G_TYPE_CHECK_CLASS_TYPE ((k), KRAKEN_TYPE_NOTEBOOK))
#define KRAKEN_NOTEBOOK_GET_CLASS(o)	(G_TYPE_INSTANCE_GET_CLASS ((o), KRAKEN_TYPE_NOTEBOOK, KrakenNotebookClass))

typedef struct _KrakenNotebookClass	KrakenNotebookClass;
typedef struct _KrakenNotebook		KrakenNotebook;

struct _KrakenNotebook
{
	GtkNotebook parent;
};

struct _KrakenNotebookClass
{
        GtkNotebookClass parent_class;

	/* Signals */
	void	 (* tab_close_request)  (KrakenNotebook *notebook,
					 KrakenWindowSlot *slot);
};

GType		kraken_notebook_get_type		(void);

int		kraken_notebook_add_tab	(KrakenNotebook *nb,
						 KrakenWindowSlot *slot,
						 int position,
						 gboolean jump_to);
gint		kraken_notebook_find_tab_num_at_pos (KrakenNotebook *nb,
						   gint 	 abs_x,
						   gint 	 abs_y);
	
void		kraken_notebook_set_show_tabs	(KrakenNotebook *nb,
						 gboolean show_tabs);

void		kraken_notebook_set_dnd_enabled (KrakenNotebook *nb,
						   gboolean enabled);
void		kraken_notebook_sync_tab_label (KrakenNotebook *nb,
						  KrakenWindowSlot *slot);
void		kraken_notebook_sync_loading   (KrakenNotebook *nb,
						  KrakenWindowSlot *slot);

void		kraken_notebook_reorder_child_relative (KrakenNotebook *notebook,
						      int	    page_num,
						      int 	    offset);
void		kraken_notebook_set_current_page_relative (KrakenNotebook *notebook,
							     int offset);

gboolean        kraken_notebook_can_reorder_child_relative (KrakenNotebook *notebook,
							  int	    	page_num,
							  int 	    	offset);
gboolean        kraken_notebook_can_set_current_page_relative (KrakenNotebook *notebook,
								 int offset);

G_END_DECLS

#endif /* KRAKEN_NOTEBOOK_H */

