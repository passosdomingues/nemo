/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* KrakenUndoManager - Manages undo and redo transactions.
 *                       This is the public interface used by the application.                      
 *
 * Copyright (C) 2000 Eazel, Inc.
 *
 * Author: Gene Z. Ragan <gzr@eazel.com>
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
 */

#ifndef KRAKEN_UNDO_MANAGER_H
#define KRAKEN_UNDO_MANAGER_H

#include <libkraken-private/kraken-undo.h>

#define KRAKEN_TYPE_UNDO_MANAGER kraken_undo_manager_get_type()
#define KRAKEN_UNDO_MANAGER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_UNDO_MANAGER, KrakenUndoManager))
#define KRAKEN_UNDO_MANAGER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_UNDO_MANAGER, KrakenUndoManagerClass))
#define KRAKEN_IS_UNDO_MANAGER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_UNDO_MANAGER))
#define KRAKEN_IS_UNDO_MANAGER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_UNDO_MANAGER))
#define KRAKEN_UNDO_MANAGER_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_UNDO_MANAGER, KrakenUndoManagerClass))
	
typedef struct KrakenUndoManagerDetails KrakenUndoManagerDetails;

typedef struct {
	GObject parent;
	KrakenUndoManagerDetails *details;
} KrakenUndoManager;

typedef struct {
	GObjectClass parent_slot;
	void (* changed) (GObject *object, gpointer data);
} KrakenUndoManagerClass;

GType                kraken_undo_manager_get_type                           (void);
KrakenUndoManager *kraken_undo_manager_new                                (void);

/* Undo operations. */
void                 kraken_undo_manager_undo                               (KrakenUndoManager *undo_manager);

/* Attach the undo manager to a Gtk object so that object and the widgets inside it can participate in undo. */
void                 kraken_undo_manager_attach                             (KrakenUndoManager *manager,
									       GObject             *object);

void		kraken_undo_manager_append (KrakenUndoManager *manager,
					      KrakenUndoTransaction *transaction);
void            kraken_undo_manager_forget (KrakenUndoManager *manager,
					      KrakenUndoTransaction *transaction);

#endif /* KRAKEN_UNDO_MANAGER_H */
