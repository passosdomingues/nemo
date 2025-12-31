/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * Kraken
 *
 * Copyright (C) 2002 Sun Microsystems, Inc.
 *
 * Kraken is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * Kraken is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street - Suite 500,
 * Boston, MA 02110-1335, USA.
 * 
 * Author: Dave Camp <dave@ximian.com>
 */

/* kraken-tree-view-drag-dest.h: Handles drag and drop for treeviews which 
 *                                 contain a hierarchy of files
 */

#ifndef KRAKEN_TREE_VIEW_DRAG_DEST_H
#define KRAKEN_TREE_VIEW_DRAG_DEST_H

#include <gtk/gtk.h>

#include "kraken-file.h"

G_BEGIN_DECLS

#define KRAKEN_TYPE_TREE_VIEW_DRAG_DEST	(kraken_tree_view_drag_dest_get_type ())
#define KRAKEN_TREE_VIEW_DRAG_DEST(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_TREE_VIEW_DRAG_DEST, KrakenTreeViewDragDest))
#define KRAKEN_TREE_VIEW_DRAG_DEST_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_TREE_VIEW_DRAG_DEST, KrakenTreeViewDragDestClass))
#define KRAKEN_IS_TREE_VIEW_DRAG_DEST(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_TREE_VIEW_DRAG_DEST))
#define KRAKEN_IS_TREE_VIEW_DRAG_DEST_CLASS(klass)	(G_TYPE_CLASS_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_TREE_VIEW_DRAG_DEST))

typedef struct _KrakenTreeViewDragDest        KrakenTreeViewDragDest;
typedef struct _KrakenTreeViewDragDestClass   KrakenTreeViewDragDestClass;
typedef struct _KrakenTreeViewDragDestDetails KrakenTreeViewDragDestDetails;

struct _KrakenTreeViewDragDest {
	GObject parent;
	
	KrakenTreeViewDragDestDetails *details;
};

struct _KrakenTreeViewDragDestClass {
	GObjectClass parent;
	
	char *(*get_root_uri) (KrakenTreeViewDragDest *dest);
	KrakenFile *(*get_file_for_path) (KrakenTreeViewDragDest *dest,
					    GtkTreePath *path);
	void (*move_copy_items) (KrakenTreeViewDragDest *dest,
				 const GList *item_uris,
				 const char *target_uri,
				 GdkDragAction action,
				 int x,
				 int y);
	void (* handle_netscape_url) (KrakenTreeViewDragDest *dest,
				 const char *url,
				 const char *target_uri,
				 GdkDragAction action,
				 int x,
				 int y);
	void (* handle_uri_list) (KrakenTreeViewDragDest *dest,
				  const char *uri_list,
				  const char *target_uri,
				  GdkDragAction action,
				  int x,
				  int y);
	void (* handle_text)    (KrakenTreeViewDragDest *dest,
				  const char *text,
				  const char *target_uri,
				  GdkDragAction action,
				  int x,
				  int y);
	void (* handle_raw)    (KrakenTreeViewDragDest *dest,
				  char *raw_data,
				  int length,
				  const char *target_uri,
				  const char *direct_save_uri,
				  GdkDragAction action,
				  int x,
				  int y);
};

GType                     kraken_tree_view_drag_dest_get_type (void);
KrakenTreeViewDragDest *kraken_tree_view_drag_dest_new      (GtkTreeView *tree_view, gboolean strict_drop);

G_END_DECLS

#endif
