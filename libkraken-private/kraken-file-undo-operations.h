/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* kraken-file-undo-operations.h - Manages undo/redo of file operations
 *
 * Copyright (C) 2007-2011 Amos Brocco
 * Copyright (C) 2010 Red Hat, Inc.
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
 * Authors: Amos Brocco <amos.brocco@gmail.com>
 *          Cosimo Cecchi <cosimoc@redhat.com>
 *
 */

#ifndef __KRAKEN_FILE_UNDO_OPERATIONS_H__
#define __KRAKEN_FILE_UNDO_OPERATIONS_H__

#include <gio/gio.h>
#include <gtk/gtk.h>

typedef enum {
	KRAKEN_FILE_UNDO_OP_COPY,
	KRAKEN_FILE_UNDO_OP_DUPLICATE,
	KRAKEN_FILE_UNDO_OP_MOVE,
	KRAKEN_FILE_UNDO_OP_RENAME,
	KRAKEN_FILE_UNDO_OP_CREATE_EMPTY_FILE,
	KRAKEN_FILE_UNDO_OP_CREATE_FILE_FROM_TEMPLATE,
	KRAKEN_FILE_UNDO_OP_CREATE_FOLDER,
	KRAKEN_FILE_UNDO_OP_MOVE_TO_TRASH,
	KRAKEN_FILE_UNDO_OP_RESTORE_FROM_TRASH,
	KRAKEN_FILE_UNDO_OP_CREATE_LINK,
	KRAKEN_FILE_UNDO_OP_RECURSIVE_SET_PERMISSIONS,
	KRAKEN_FILE_UNDO_OP_SET_PERMISSIONS,
	KRAKEN_FILE_UNDO_OP_CHANGE_GROUP,
	KRAKEN_FILE_UNDO_OP_CHANGE_OWNER,
	KRAKEN_FILE_UNDO_OP_NUM_TYPES,
} KrakenFileUndoOp;

#define KRAKEN_TYPE_FILE_UNDO_INFO         (kraken_file_undo_info_get_type ())
#define KRAKEN_FILE_UNDO_INFO(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), KRAKEN_TYPE_FILE_UNDO_INFO, KrakenFileUndoInfo))
#define KRAKEN_FILE_UNDO_INFO_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), KRAKEN_TYPE_FILE_UNDO_INFO, KrakenFileUndoInfoClass))
#define KRAKEN_IS_FILE_UNDO_INFO(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), KRAKEN_TYPE_FILE_UNDO_INFO))
#define KRAKEN_IS_FILE_UNDO_INFO_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), KRAKEN_TYPE_FILE_UNDO_INFO))
#define KRAKEN_FILE_UNDO_INFO_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), KRAKEN_TYPE_FILE_UNDO_INFO, KrakenFileUndoInfoClass))

typedef struct _KrakenFileUndoInfo      KrakenFileUndoInfo;
typedef struct _KrakenFileUndoInfoClass KrakenFileUndoInfoClass;
typedef struct _KrakenFileUndoInfoDetails KrakenFileUndoInfoDetails;

struct _KrakenFileUndoInfo {
	GObject parent;
	KrakenFileUndoInfoDetails *priv;
};

struct _KrakenFileUndoInfoClass {
	GObjectClass parent_class;

	void (* undo_func) (KrakenFileUndoInfo *self,
			    GtkWindow            *parent_window);
	void (* redo_func) (KrakenFileUndoInfo *self,
			    GtkWindow            *parent_window);

	void (* strings_func) (KrakenFileUndoInfo *self,
			       gchar **undo_label,
			       gchar **undo_description,
			       gchar **redo_label,
			       gchar **redo_description);
};

GType kraken_file_undo_info_get_type (void) G_GNUC_CONST;

void kraken_file_undo_info_apply_async (KrakenFileUndoInfo *self,
					  gboolean undo,
					  GtkWindow *parent_window,
					  GAsyncReadyCallback callback,
					  gpointer user_data);
gboolean kraken_file_undo_info_apply_finish (KrakenFileUndoInfo *self,
					       GAsyncResult *res,
					       gboolean *user_cancel,
					       GError **error);

void kraken_file_undo_info_get_strings (KrakenFileUndoInfo *self,
					  gchar **undo_label,
					  gchar **undo_description,
					  gchar **redo_label,
					  gchar **redo_description);

/* copy/move/duplicate/link/restore from trash */
#define KRAKEN_TYPE_FILE_UNDO_INFO_EXT         (kraken_file_undo_info_ext_get_type ())
#define KRAKEN_FILE_UNDO_INFO_EXT(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), KRAKEN_TYPE_FILE_UNDO_INFO_EXT, KrakenFileUndoInfoExt))
#define KRAKEN_FILE_UNDO_INFO_EXT_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), KRAKEN_TYPE_FILE_UNDO_INFO_EXT, KrakenFileUndoInfoExtClass))
#define KRAKEN_IS_FILE_UNDO_INFO_EXT(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), KRAKEN_TYPE_FILE_UNDO_INFO_EXT))
#define KRAKEN_IS_FILE_UNDO_INFO_EXT_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), KRAKEN_TYPE_FILE_UNDO_INFO_EXT))
#define KRAKEN_FILE_UNDO_INFO_EXT_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), KRAKEN_TYPE_FILE_UNDO_INFO_EXT, KrakenFileUndoInfoExtClass))

typedef struct _KrakenFileUndoInfoExt      KrakenFileUndoInfoExt;
typedef struct _KrakenFileUndoInfoExtClass KrakenFileUndoInfoExtClass;
typedef struct _KrakenFileUndoInfoExtDetails KrakenFileUndoInfoExtDetails;

struct _KrakenFileUndoInfoExt {
	KrakenFileUndoInfo parent;
	KrakenFileUndoInfoExtDetails *priv;
};

struct _KrakenFileUndoInfoExtClass {
	KrakenFileUndoInfoClass parent_class;
};

GType kraken_file_undo_info_ext_get_type (void) G_GNUC_CONST;
KrakenFileUndoInfo *kraken_file_undo_info_ext_new (KrakenFileUndoOp op_type,
						       gint item_count,
						       GFile *src_dir,
						       GFile *target_dir);
void kraken_file_undo_info_ext_add_origin_target_pair (KrakenFileUndoInfoExt *self,
							 GFile                   *origin,
							 GFile                   *target);

/* create new file/folder */
#define KRAKEN_TYPE_FILE_UNDO_INFO_CREATE         (kraken_file_undo_info_create_get_type ())
#define KRAKEN_FILE_UNDO_INFO_CREATE(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), KRAKEN_TYPE_FILE_UNDO_INFO_CREATE, KrakenFileUndoInfoCreate))
#define KRAKEN_FILE_UNDO_INFO_CREATE_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), KRAKEN_TYPE_FILE_UNDO_INFO_CREATE, KrakenFileUndoInfoCreateClass))
#define KRAKEN_IS_FILE_UNDO_INFO_CREATE(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), KRAKEN_TYPE_FILE_UNDO_INFO_CREATE))
#define KRAKEN_IS_FILE_UNDO_INFO_CREATE_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), KRAKEN_TYPE_FILE_UNDO_INFO_CREATE))
#define KRAKEN_FILE_UNDO_INFO_CREATE_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), KRAKEN_TYPE_FILE_UNDO_INFO_CREATE, KrakenFileUndoInfoCreateClass))

typedef struct _KrakenFileUndoInfoCreate      KrakenFileUndoInfoCreate;
typedef struct _KrakenFileUndoInfoCreateClass KrakenFileUndoInfoCreateClass;
typedef struct _KrakenFileUndoInfoCreateDetails KrakenFileUndoInfoCreateDetails;

struct _KrakenFileUndoInfoCreate {
	KrakenFileUndoInfo parent;
	KrakenFileUndoInfoCreateDetails *priv;
};

struct _KrakenFileUndoInfoCreateClass {
	KrakenFileUndoInfoClass parent_class;
};

GType kraken_file_undo_info_create_get_type (void) G_GNUC_CONST;
KrakenFileUndoInfo *kraken_file_undo_info_create_new (KrakenFileUndoOp op_type);
void kraken_file_undo_info_create_set_data (KrakenFileUndoInfoCreate *self,
					      GFile                      *file,
					      const char                 *template,
					      gint                        length);

/* rename */
#define KRAKEN_TYPE_FILE_UNDO_INFO_RENAME         (kraken_file_undo_info_rename_get_type ())
#define KRAKEN_FILE_UNDO_INFO_RENAME(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), KRAKEN_TYPE_FILE_UNDO_INFO_RENAME, KrakenFileUndoInfoRename))
#define KRAKEN_FILE_UNDO_INFO_RENAME_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), KRAKEN_TYPE_FILE_UNDO_INFO_RENAME, KrakenFileUndoInfoRenameClass))
#define KRAKEN_IS_FILE_UNDO_INFO_RENAME(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), KRAKEN_TYPE_FILE_UNDO_INFO_RENAME))
#define KRAKEN_IS_FILE_UNDO_INFO_RENAME_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), KRAKEN_TYPE_FILE_UNDO_INFO_RENAME))
#define KRAKEN_FILE_UNDO_INFO_RENAME_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), KRAKEN_TYPE_FILE_UNDO_INFO_RENAME, KrakenFileUndoInfoRenameClass))

typedef struct _KrakenFileUndoInfoRename      KrakenFileUndoInfoRename;
typedef struct _KrakenFileUndoInfoRenameClass KrakenFileUndoInfoRenameClass;
typedef struct _KrakenFileUndoInfoRenameDetails KrakenFileUndoInfoRenameDetails;

struct _KrakenFileUndoInfoRename {
	KrakenFileUndoInfo parent;
	KrakenFileUndoInfoRenameDetails *priv;
};

struct _KrakenFileUndoInfoRenameClass {
	KrakenFileUndoInfoClass parent_class;
};

GType kraken_file_undo_info_rename_get_type (void) G_GNUC_CONST;
KrakenFileUndoInfo *kraken_file_undo_info_rename_new (void);
void kraken_file_undo_info_rename_set_data_pre (KrakenFileUndoInfoRename *self,
						  GFile                      *old_file,
						  gchar                      *old_display_name,
						  gchar                      *new_display_name);
void kraken_file_undo_info_rename_set_data_post (KrakenFileUndoInfoRename *self,
						   GFile                      *new_file);

/* trash */
#define KRAKEN_TYPE_FILE_UNDO_INFO_TRASH         (kraken_file_undo_info_trash_get_type ())
#define KRAKEN_FILE_UNDO_INFO_TRASH(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), KRAKEN_TYPE_FILE_UNDO_INFO_TRASH, KrakenFileUndoInfoTrash))
#define KRAKEN_FILE_UNDO_INFO_TRASH_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), KRAKEN_TYPE_FILE_UNDO_INFO_TRASH, KrakenFileUndoInfoTrashClass))
#define KRAKEN_IS_FILE_UNDO_INFO_TRASH(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), KRAKEN_TYPE_FILE_UNDO_INFO_TRASH))
#define KRAKEN_IS_FILE_UNDO_INFO_TRASH_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), KRAKEN_TYPE_FILE_UNDO_INFO_TRASH))
#define KRAKEN_FILE_UNDO_INFO_TRASH_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), KRAKEN_TYPE_FILE_UNDO_INFO_TRASH, KrakenFileUndoInfoTrashClass))

typedef struct _KrakenFileUndoInfoTrash      KrakenFileUndoInfoTrash;
typedef struct _KrakenFileUndoInfoTrashClass KrakenFileUndoInfoTrashClass;
typedef struct _KrakenFileUndoInfoTrashDetails KrakenFileUndoInfoTrashDetails;

struct _KrakenFileUndoInfoTrash {
	KrakenFileUndoInfo parent;
	KrakenFileUndoInfoTrashDetails *priv;
};

struct _KrakenFileUndoInfoTrashClass {
	KrakenFileUndoInfoClass parent_class;
};

GType kraken_file_undo_info_trash_get_type (void) G_GNUC_CONST;
KrakenFileUndoInfo *kraken_file_undo_info_trash_new (gint item_count);
void kraken_file_undo_info_trash_add_file (KrakenFileUndoInfoTrash *self,
					     GFile                     *file);

/* recursive permissions */
#define KRAKEN_TYPE_FILE_UNDO_INFO_REC_PERMISSIONS         (kraken_file_undo_info_rec_permissions_get_type ())
#define KRAKEN_FILE_UNDO_INFO_REC_PERMISSIONS(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), KRAKEN_TYPE_FILE_UNDO_INFO_REC_PERMISSIONS, KrakenFileUndoInfoRecPermissions))
#define KRAKEN_FILE_UNDO_INFO_REC_PERMISSIONS_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), KRAKEN_TYPE_FILE_UNDO_INFO_REC_PERMISSIONS, KrakenFileUndoInfoRecPermissionsClass))
#define KRAKEN_IS_FILE_UNDO_INFO_REC_PERMISSIONS(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), KRAKEN_TYPE_FILE_UNDO_INFO_REC_PERMISSIONS))
#define KRAKEN_IS_FILE_UNDO_INFO_REC_PERMISSIONS_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), KRAKEN_TYPE_FILE_UNDO_INFO_REC_PERMISSIONS))
#define KRAKEN_FILE_UNDO_INFO_REC_PERMISSIONS_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), KRAKEN_TYPE_FILE_UNDO_INFO_REC_PERMISSIONS, KrakenFileUndoInfoRecPermissionsClass))

typedef struct _KrakenFileUndoInfoRecPermissions      KrakenFileUndoInfoRecPermissions;
typedef struct _KrakenFileUndoInfoRecPermissionsClass KrakenFileUndoInfoRecPermissionsClass;
typedef struct _KrakenFileUndoInfoRecPermissionsDetails KrakenFileUndoInfoRecPermissionsDetails;

struct _KrakenFileUndoInfoRecPermissions {
	KrakenFileUndoInfo parent;
	KrakenFileUndoInfoRecPermissionsDetails *priv;
};

struct _KrakenFileUndoInfoRecPermissionsClass {
	KrakenFileUndoInfoClass parent_class;
};

GType kraken_file_undo_info_rec_permissions_get_type (void) G_GNUC_CONST;
KrakenFileUndoInfo *kraken_file_undo_info_rec_permissions_new (GFile   *dest,
								   guint32 file_permissions,
								   guint32 file_mask,
								   guint32 dir_permissions,
								   guint32 dir_mask);
void kraken_file_undo_info_rec_permissions_add_file (KrakenFileUndoInfoRecPermissions *self,
						       GFile                              *file,
						       guint32                             permission);

/* single file change permissions */
#define KRAKEN_TYPE_FILE_UNDO_INFO_PERMISSIONS         (kraken_file_undo_info_permissions_get_type ())
#define KRAKEN_FILE_UNDO_INFO_PERMISSIONS(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), KRAKEN_TYPE_FILE_UNDO_INFO_PERMISSIONS, KrakenFileUndoInfoPermissions))
#define KRAKEN_FILE_UNDO_INFO_PERMISSIONS_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), KRAKEN_TYPE_FILE_UNDO_INFO_PERMISSIONS, KrakenFileUndoInfoPermissionsClass))
#define KRAKEN_IS_FILE_UNDO_INFO_PERMISSIONS(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), KRAKEN_TYPE_FILE_UNDO_INFO_PERMISSIONS))
#define KRAKEN_IS_FILE_UNDO_INFO_PERMISSIONS_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), KRAKEN_TYPE_FILE_UNDO_INFO_PERMISSIONS))
#define KRAKEN_FILE_UNDO_INFO_PERMISSIONS_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), KRAKEN_TYPE_FILE_UNDO_INFO_PERMISSIONS, KrakenFileUndoInfoPermissionsClass))

typedef struct _KrakenFileUndoInfoPermissions      KrakenFileUndoInfoPermissions;
typedef struct _KrakenFileUndoInfoPermissionsClass KrakenFileUndoInfoPermissionsClass;
typedef struct _KrakenFileUndoInfoPermissionsDetails KrakenFileUndoInfoPermissionsDetails;

struct _KrakenFileUndoInfoPermissions {
	KrakenFileUndoInfo parent;
	KrakenFileUndoInfoPermissionsDetails *priv;
};

struct _KrakenFileUndoInfoPermissionsClass {
	KrakenFileUndoInfoClass parent_class;
};

GType kraken_file_undo_info_permissions_get_type (void) G_GNUC_CONST;
KrakenFileUndoInfo *kraken_file_undo_info_permissions_new (GFile   *file,
							       guint32  current_permissions,
							       guint32  new_permissions);

/* group and owner change */
#define KRAKEN_TYPE_FILE_UNDO_INFO_OWNERSHIP         (kraken_file_undo_info_ownership_get_type ())
#define KRAKEN_FILE_UNDO_INFO_OWNERSHIP(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), KRAKEN_TYPE_FILE_UNDO_INFO_OWNERSHIP, KrakenFileUndoInfoOwnership))
#define KRAKEN_FILE_UNDO_INFO_OWNERSHIP_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), KRAKEN_TYPE_FILE_UNDO_INFO_OWNERSHIP, KrakenFileUndoInfoOwnershipClass))
#define KRAKEN_IS_FILE_UNDO_INFO_OWNERSHIP(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), KRAKEN_TYPE_FILE_UNDO_INFO_OWNERSHIP))
#define KRAKEN_IS_FILE_UNDO_INFO_OWNERSHIP_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), KRAKEN_TYPE_FILE_UNDO_INFO_OWNERSHIP))
#define KRAKEN_FILE_UNDO_INFO_OWNERSHIP_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), KRAKEN_TYPE_FILE_UNDO_INFO_OWNERSHIP, KrakenFileUndoInfoOwnershipClass))

typedef struct _KrakenFileUndoInfoOwnership      KrakenFileUndoInfoOwnership;
typedef struct _KrakenFileUndoInfoOwnershipClass KrakenFileUndoInfoOwnershipClass;
typedef struct _KrakenFileUndoInfoOwnershipDetails KrakenFileUndoInfoOwnershipDetails;

struct _KrakenFileUndoInfoOwnership {
	KrakenFileUndoInfo parent;
	KrakenFileUndoInfoOwnershipDetails *priv;
};

struct _KrakenFileUndoInfoOwnershipClass {
	KrakenFileUndoInfoClass parent_class;
};

GType kraken_file_undo_info_ownership_get_type (void) G_GNUC_CONST;
KrakenFileUndoInfo *kraken_file_undo_info_ownership_new (KrakenFileUndoOp  op_type,
							     GFile              *file,
							     const char         *current_data,
							     const char         *new_data);

#endif /* __KRAKEN_FILE_UNDO_OPERATIONS_H__ */
