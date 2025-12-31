/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* fm-list-model.h - a GtkTreeModel for file lists. 

   Copyright (C) 2001, 2002 Anders Carlsson

   The Gnome Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The Gnome Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 51 Franklin Street - Suite 500,
   Boston, MA 02110-1335, USA.

   Authors: Anders Carlsson <andersca@gnu.org>
*/

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <libkraken-private/kraken-file.h>
#include <libkraken-private/kraken-directory.h>
#include <libkraken-extension/kraken-column.h>

#ifndef KRAKEN_LIST_MODEL_H
#define KRAKEN_LIST_MODEL_H

#define KRAKEN_TYPE_LIST_MODEL kraken_list_model_get_type()
#define KRAKEN_LIST_MODEL(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_LIST_MODEL, KrakenListModel))
#define KRAKEN_LIST_MODEL_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_LIST_MODEL, KrakenListModelClass))
#define KRAKEN_IS_LIST_MODEL(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_LIST_MODEL))
#define KRAKEN_IS_LIST_MODEL_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_LIST_MODEL))
#define KRAKEN_LIST_MODEL_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_LIST_MODEL, KrakenListModelClass))

enum {
	KRAKEN_LIST_MODEL_FILE_COLUMN,
	KRAKEN_LIST_MODEL_SUBDIRECTORY_COLUMN,
	KRAKEN_LIST_MODEL_SMALLEST_ICON_COLUMN,
	KRAKEN_LIST_MODEL_SMALLER_ICON_COLUMN,
	KRAKEN_LIST_MODEL_SMALL_ICON_COLUMN,
	KRAKEN_LIST_MODEL_STANDARD_ICON_COLUMN,
	KRAKEN_LIST_MODEL_LARGE_ICON_COLUMN,
	KRAKEN_LIST_MODEL_LARGER_ICON_COLUMN,
	KRAKEN_LIST_MODEL_LARGEST_ICON_COLUMN,
	KRAKEN_LIST_MODEL_FILE_NAME_IS_EDITABLE_COLUMN,
    KRAKEN_LIST_MODEL_TEXT_WEIGHT_COLUMN,
    KRAKEN_LIST_MODEL_ICON_SHOWN,
	KRAKEN_LIST_MODEL_NUM_COLUMNS
};

typedef struct KrakenListModelDetails KrakenListModelDetails;

typedef struct KrakenListModel {
	GObject parent_instance;
	KrakenListModelDetails *details;
} KrakenListModel;

typedef struct {
	GObjectClass parent_class;

	void (* subdirectory_unloaded)(KrakenListModel *model,
				       KrakenDirectory *subdirectory);
} KrakenListModelClass;

GType    kraken_list_model_get_type                          (void);
gboolean kraken_list_model_add_file                          (KrakenListModel          *model,
								KrakenFile         *file,
								KrakenDirectory    *directory);
void     kraken_list_model_file_changed                      (KrakenListModel          *model,
								KrakenFile         *file,
								KrakenDirectory    *directory);
gboolean kraken_list_model_is_empty                          (KrakenListModel          *model);
guint    kraken_list_model_get_length                        (KrakenListModel          *model);
void     kraken_list_model_remove_file                       (KrakenListModel          *model,
								KrakenFile         *file,
								KrakenDirectory    *directory);
void     kraken_list_model_clear                             (KrakenListModel          *model);
gboolean kraken_list_model_get_tree_iter_from_file           (KrakenListModel          *model,
								KrakenFile         *file,
								KrakenDirectory    *directory,
								GtkTreeIter          *iter);
GList *  kraken_list_model_get_all_iters_for_file            (KrakenListModel          *model,
								KrakenFile         *file);
gboolean kraken_list_model_get_first_iter_for_file           (KrakenListModel          *model,
								KrakenFile         *file,
								GtkTreeIter          *iter);
void     kraken_list_model_set_should_sort_directories_first (KrakenListModel          *model,
								gboolean              sort_directories_first);
void     kraken_list_model_set_should_sort_favorites_first (KrakenListModel          *model,
								gboolean              sort_favorites_first);
int      kraken_list_model_get_sort_column_id_from_attribute (KrakenListModel *model,
								GQuark       attribute);
GQuark   kraken_list_model_get_attribute_from_sort_column_id (KrakenListModel *model,
								int sort_column_id);
void     kraken_list_model_sort_files                        (KrakenListModel *model,
								GList **files);

KrakenZoomLevel kraken_list_model_get_zoom_level_from_column_id (int               column);
int               kraken_list_model_get_column_id_from_zoom_level (KrakenZoomLevel zoom_level);

KrakenFile *    kraken_list_model_file_for_path (KrakenListModel *model, GtkTreePath *path);
gboolean          kraken_list_model_load_subdirectory (KrakenListModel *model, GtkTreePath *path, KrakenDirectory **directory);
void              kraken_list_model_unload_subdirectory (KrakenListModel *model, GtkTreeIter *iter);

void              kraken_list_model_set_drag_view (KrakenListModel *model,
						     GtkTreeView *view,
						     int begin_x, 
						     int begin_y);

GtkTargetList *   kraken_list_model_get_drag_target_list (void);

int               kraken_list_model_compare_func (KrakenListModel *model,
						    KrakenFile *file1,
						    KrakenFile *file2);


int               kraken_list_model_add_column (KrakenListModel *model,
						  KrakenColumn *column);
int               kraken_list_model_get_column_number (KrakenListModel *model,
							 const char *column_name);

void              kraken_list_model_subdirectory_done_loading (KrakenListModel       *model,
								 KrakenDirectory *directory);

void              kraken_list_model_set_highlight_for_files (KrakenListModel *model,
							       GList *files);

void              kraken_list_model_set_temporarily_disable_sort (KrakenListModel *model, gboolean disable);
gboolean          kraken_list_model_get_temporarily_disable_sort (KrakenListModel *model);
void              kraken_list_model_set_expanding                (KrakenListModel *model, KrakenDirectory *directory);
void              kraken_list_model_set_view_directory           (KrakenListModel *model, KrakenDirectory *dir);
#endif /* KRAKEN_LIST_MODEL_H */
