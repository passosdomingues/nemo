/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * Kraken
 *
 * Copyright (C) 1999, 2000 Eazel, Inc.
 *
 * Kraken is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * Kraken is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Suite 500, MA 02110-1335, USA.
 *
 * Authors: John Sullivan <sullivan@eazel.com>
 */

/* kraken-bookmark-list.h - interface for centralized list of bookmarks.
 */

#ifndef KRAKEN_BOOKMARK_LIST_H
#define KRAKEN_BOOKMARK_LIST_H

#include <libkraken-private/kraken-bookmark.h>
#include <gio/gio.h>

typedef struct KrakenBookmarkList KrakenBookmarkList;
typedef struct KrakenBookmarkListClass KrakenBookmarkListClass;

#define KRAKEN_TYPE_BOOKMARK_LIST kraken_bookmark_list_get_type()
#define KRAKEN_BOOKMARK_LIST(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_BOOKMARK_LIST, KrakenBookmarkList))
#define KRAKEN_BOOKMARK_LIST_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_BOOKMARK_LIST, KrakenBookmarkListClass))
#define KRAKEN_IS_BOOKMARK_LIST(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_BOOKMARK_LIST))
#define KRAKEN_IS_BOOKMARK_LIST_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_BOOKMARK_LIST))
#define KRAKEN_BOOKMARK_LIST_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_BOOKMARK_LIST, KrakenBookmarkListClass))

struct KrakenBookmarkList {
	GObject object;

	GList *list; 
	GFileMonitor *monitor;
	GQueue *pending_ops;
    GVolumeMonitor *volume_monitor;

    guint idle_notify_id;
};

struct KrakenBookmarkListClass {
	GObjectClass parent_class;
	void (* changed) (KrakenBookmarkList *bookmarks);
};

GType                   kraken_bookmark_list_get_type            (void);
KrakenBookmarkList *  kraken_bookmark_list_get_default                 (void);
void                    kraken_bookmark_list_append              (KrakenBookmarkList   *bookmarks,
								    KrakenBookmark *bookmark);
gboolean                kraken_bookmark_list_contains            (KrakenBookmarkList   *bookmarks,
								    KrakenBookmark *bookmark);
void                    kraken_bookmark_list_delete_item_at      (KrakenBookmarkList   *bookmarks,
								    guint                   index);
void                    kraken_bookmark_list_delete_items_with_uri (KrakenBookmarkList *bookmarks,
								    const char		   *uri);
void                    kraken_bookmark_list_insert_item         (KrakenBookmarkList   *bookmarks,
								    KrakenBookmark *bookmark,
								    guint                   index);
GList *                 kraken_bookmark_list_get_for_uri         (KrakenBookmarkList   *bookmarks,
                                                                const char *uri);
guint                   kraken_bookmark_list_length              (KrakenBookmarkList   *bookmarks);
KrakenBookmark *      kraken_bookmark_list_item_at             (KrakenBookmarkList   *bookmarks,
								    guint                   index);
void                    kraken_bookmark_list_move_item           (KrakenBookmarkList *bookmarks,
								    guint                 index,
								    guint                 destination);
void                    kraken_bookmark_list_sort_ascending           (KrakenBookmarkList *bookmarks);
void                    kraken_bookmark_list_set_window_geometry (KrakenBookmarkList   *bookmarks,
								    const char             *geometry);
const char *            kraken_bookmark_list_get_window_geometry (KrakenBookmarkList   *bookmarks);

#endif /* KRAKEN_BOOKMARK_LIST_H */
