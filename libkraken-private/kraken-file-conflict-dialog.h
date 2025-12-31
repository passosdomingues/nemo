/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* kraken-file-conflict-dialog: dialog that handles file conflicts
   during transfer operations.

   Copyright (C) 2008, Cosimo Cecchi

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.
   
   You should have received a copy of the GNU General Public
   License along with this program; if not, write to the
   Free Software Foundation, Inc., 51 Franklin Street - Suite 500,
   Boston, MA 02110-1335, USA.
   
   Authors: Cosimo Cecchi <cosimoc@gnome.org>
*/

#ifndef KRAKEN_FILE_CONFLICT_DIALOG_H
#define KRAKEN_FILE_CONFLICT_DIALOG_H

#include <glib-object.h>
#include <gio/gio.h>
#include <gtk/gtk.h>

#define KRAKEN_TYPE_FILE_CONFLICT_DIALOG \
	(kraken_file_conflict_dialog_get_type ())
#define KRAKEN_FILE_CONFLICT_DIALOG(o) \
	(G_TYPE_CHECK_INSTANCE_CAST ((o), KRAKEN_TYPE_FILE_CONFLICT_DIALOG,\
				     KrakenFileConflictDialog))
#define KRAKEN_FILE_CONFLICT_DIALOG_CLASS(k) \
	(G_TYPE_CHECK_CLASS_CAST((k), KRAKEN_TYPE_FILE_CONFLICT_DIALOG,\
				 KrakenFileConflictDialogClass))
#define KRAKEN_IS_FILE_CONFLICT_DIALOG(o) \
	(G_TYPE_CHECK_INSTANCE_TYPE ((o), KRAKEN_TYPE_FILE_CONFLICT_DIALOG))
#define KRAKEN_IS_FILE_CONFLICT_DIALOG_CLASS(k) \
	(G_TYPE_CHECK_CLASS_TYPE ((k), KRAKEN_TYPE_FILE_CONFLICT_DIALOG))
#define KRAKEN_FILE_CONFLICT_DIALOG_GET_CLASS(o) \
	(G_TYPE_INSTANCE_GET_CLASS ((o), KRAKEN_TYPE_FILE_CONFLICT_DIALOG,\
				    KrakenFileConflictDialogClass))

typedef struct _KrakenFileConflictDialog        KrakenFileConflictDialog;
typedef struct _KrakenFileConflictDialogClass   KrakenFileConflictDialogClass;
typedef struct _KrakenFileConflictDialogDetails KrakenFileConflictDialogDetails;

struct _KrakenFileConflictDialog {
	GtkDialog parent;
	KrakenFileConflictDialogDetails *details;
};

struct _KrakenFileConflictDialogClass {
	GtkDialogClass parent_class;
};

enum
{
	CONFLICT_RESPONSE_SKIP = 1,
	CONFLICT_RESPONSE_AUTO_RENAME = 2,
	CONFLICT_RESPONSE_REPLACE = 3,
	CONFLICT_RESPONSE_RENAME = 4
};

GType kraken_file_conflict_dialog_get_type (void) G_GNUC_CONST;

GtkWidget* kraken_file_conflict_dialog_new              (GtkWindow *parent,
							   GFile *source,
							   GFile *destination,
							   GFile *dest_dir);
char*      kraken_file_conflict_dialog_get_new_name     (KrakenFileConflictDialog *dialog);
gboolean   kraken_file_conflict_dialog_get_apply_to_all (KrakenFileConflictDialog *dialog);

#endif /* KRAKEN_FILE_CONFLICT_DIALOG_H */
