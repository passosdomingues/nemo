/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* KrakenUndoTransaction - An object for an undoable transaction.
 *                           Used internally by undo machinery.
 *                           Not public.
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

#ifndef KRAKEN_UNDO_TRANSACTION_H
#define KRAKEN_UNDO_TRANSACTION_H

#include <libkraken-private/kraken-undo.h>

#define KRAKEN_TYPE_UNDO_TRANSACTION kraken_undo_transaction_get_type()
#define KRAKEN_UNDO_TRANSACTION(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_UNDO_TRANSACTION, KrakenUndoTransaction))
#define KRAKEN_UNDO_TRANSACTION_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_UNDO_TRANSACTION, KrakenUndoTransactionClass))
#define KRAKEN_IS_UNDO_TRANSACTION(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_UNDO_TRANSACTION))
#define KRAKEN_IS_UNDO_TRANSACTION_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_UNDO_TRANSACTION))
#define KRAKEN_UNDO_TRANSACTION_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_UNDO_TRANSACTION, KrakenUndoTransactionClass))

/* The typedef for KrakenUndoTransaction is in kraken-undo.h
   to avoid circular deps */
typedef struct _KrakenUndoTransactionClass KrakenUndoTransactionClass;

struct _KrakenUndoTransaction {
	GObject parent_slot;
	
	char *operation_name;
	char *undo_menu_item_label;
	char *undo_menu_item_hint;
	char *redo_menu_item_label;
	char *redo_menu_item_hint;
	GList *atom_list;

	KrakenUndoManager *owner;
};

struct _KrakenUndoTransactionClass {
	GObjectClass parent_slot;
};

GType                    kraken_undo_transaction_get_type            (void);
KrakenUndoTransaction *kraken_undo_transaction_new                 (const char              *operation_name,
									const char              *undo_menu_item_label,
									const char              *undo_menu_item_hint,
									const char              *redo_menu_item_label,
									const char              *redo_menu_item_hint);
void                     kraken_undo_transaction_add_atom            (KrakenUndoTransaction *transaction,
									const KrakenUndoAtom  *atom);
void                     kraken_undo_transaction_add_to_undo_manager (KrakenUndoTransaction *transaction,
									KrakenUndoManager     *manager);
void                     kraken_undo_transaction_unregister_object   (GObject                 *atom_target);
void                     kraken_undo_transaction_undo                (KrakenUndoTransaction *transaction);

#endif /* KRAKEN_UNDO_TRANSACTION_H */
