/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* kraken-file-undo-manager.h - Manages the undo/redo stack
 *
 * Copyright (C) 2007-2011 Amos Brocco
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street - Suite 500,
 * Boston, MA 02110-1335, USA.
 *
 * Author: Amos Brocco <amos.brocco@gmail.com>
 */

#ifndef __KRAKEN_FILE_UNDO_MANAGER_H__
#define __KRAKEN_FILE_UNDO_MANAGER_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <gio/gio.h>

#include <libkraken-private/kraken-file-undo-operations.h>

typedef struct _KrakenFileUndoManager KrakenFileUndoManager;
typedef struct _KrakenFileUndoManagerClass KrakenFileUndoManagerClass;
typedef struct _KrakenFileUndoManagerPrivate KrakenFileUndoManagerPrivate;

#define KRAKEN_TYPE_FILE_UNDO_MANAGER\
	(kraken_file_undo_manager_get_type())
#define KRAKEN_FILE_UNDO_MANAGER(object)\
	(G_TYPE_CHECK_INSTANCE_CAST((object), KRAKEN_TYPE_FILE_UNDO_MANAGER,\
				    KrakenFileUndoManager))
#define KRAKEN_FILE_UNDO_MANAGER_CLASS(klass)\
	(G_TYPE_CHECK_CLASS_CAST((klass), KRAKEN_TYPE_FILE_UNDO_MANAGER,\
				 KrakenFileUndoManagerClass))
#define KRAKEN_IS_FILE_UNDO_MANAGER(object)\
	(G_TYPE_CHECK_INSTANCE_TYPE((object), KRAKEN_TYPE_FILE_UNDO_MANAGER))
#define KRAKEN_IS_FILE_UNDO_MANAGER_CLASS(klass)\
	(G_TYPE_CHECK_CLASS_TYPE((klass), KRAKEN_TYPE_FILE_UNDO_MANAGER))
#define KRAKEN_FILE_UNDO_MANAGER_GET_CLASS(object)\
	(G_TYPE_INSTANCE_GET_CLASS((object), KRAKEN_TYPE_FILE_UNDO_MANAGER,\
				   KrakenFileUndoManagerClass))

typedef enum {
	KRAKEN_FILE_UNDO_MANAGER_STATE_NONE,
	KRAKEN_FILE_UNDO_MANAGER_STATE_UNDO,
	KRAKEN_FILE_UNDO_MANAGER_STATE_REDO
} KrakenFileUndoManagerState;

struct _KrakenFileUndoManager {
	GObject parent_instance;

	/* < private > */
	KrakenFileUndoManagerPrivate* priv;
};

struct _KrakenFileUndoManagerClass {
	GObjectClass parent_class;
};

GType kraken_file_undo_manager_get_type (void) G_GNUC_CONST;

KrakenFileUndoManager * kraken_file_undo_manager_get (void);

void kraken_file_undo_manager_set_action (KrakenFileUndoInfo *info);
KrakenFileUndoInfo *kraken_file_undo_manager_get_action (void);

KrakenFileUndoManagerState kraken_file_undo_manager_get_state (void);

void kraken_file_undo_manager_undo (GtkWindow *parent_window);
void kraken_file_undo_manager_redo (GtkWindow *parent_window);

void kraken_file_undo_manager_push_flag (void);
gboolean kraken_file_undo_manager_pop_flag (void);

#endif /* __KRAKEN_FILE_UNDO_MANAGER_H__ */
