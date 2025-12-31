/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* kraken-bookmark.h - implementation of individual bookmarks.
 *
 * Copyright (C) 1999, 2000 Eazel, Inc.
 * Copyright (C) 2011, Red Hat, Inc.
 *
 * The Gnome Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Gnome Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with the Gnome Library; see the file COPYING.LIB.  If not,
 * write to the Free Software Foundation, Inc., 51 Franklin Street - Suite 500,
 * Boston, MA 02110-1335, USA.
 *
 * Authors: John Sullivan <sullivan@eazel.com>
 *          Cosimo Cecchi <cosimoc@redhat.com>
 */

#ifndef KRAKEN_BOOKMARK_H
#define KRAKEN_BOOKMARK_H

#include <gtk/gtk.h>
#include <gio/gio.h>
typedef struct KrakenBookmark KrakenBookmark;

#define KRAKEN_TYPE_BOOKMARK kraken_bookmark_get_type()
#define KRAKEN_BOOKMARK(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_BOOKMARK, KrakenBookmark))
#define KRAKEN_BOOKMARK_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_BOOKMARK, KrakenBookmarkClass))
#define KRAKEN_IS_BOOKMARK(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_BOOKMARK))
#define KRAKEN_IS_BOOKMARK_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_BOOKMARK))
#define KRAKEN_BOOKMARK_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_BOOKMARK, KrakenBookmarkClass))

typedef struct KrakenBookmarkDetails KrakenBookmarkDetails;

struct KrakenBookmark {
	GObject object;
	KrakenBookmarkDetails *details;	
};

typedef struct
{
  gchar  *bookmark_name;
  gchar **emblems;
} KrakenBookmarkMetadata;

struct KrakenBookmarkClass {
	GObjectClass parent_class;

	/* Signals that clients can connect to. */

	/* The contents-changed signal is emitted when the bookmark's contents
	 * (custom name or URI) changed.
	 */
	void	(* contents_changed) (KrakenBookmark *bookmark);

    gboolean (* location_mounted)         (GFile *location);
};

typedef struct KrakenBookmarkClass KrakenBookmarkClass;

GType                 kraken_bookmark_get_type               (void);
KrakenBookmark *    kraken_bookmark_new                    (GFile                *location,
                                                        const char           *custom_name,
                                                        const char           *icon_name,
                                                        KrakenBookmarkMetadata *md);
KrakenBookmark *    kraken_bookmark_copy                   (KrakenBookmark      *bookmark);
const char *          kraken_bookmark_get_name               (KrakenBookmark      *bookmark);
GFile *               kraken_bookmark_get_location           (KrakenBookmark      *bookmark);
char *                kraken_bookmark_get_uri                (KrakenBookmark      *bookmark);
gchar *               kraken_bookmark_get_icon_name          (KrakenBookmark      *bookmark);
gboolean	      kraken_bookmark_get_has_custom_name    (KrakenBookmark      *bookmark);		
void                  kraken_bookmark_set_custom_name        (KrakenBookmark      *bookmark,
								const char            *new_name);		
gboolean              kraken_bookmark_uri_get_exists         (KrakenBookmark      *bookmark);
int                   kraken_bookmark_compare_with           (gconstpointer          a,
								gconstpointer          b);
int                   kraken_bookmark_compare_uris           (gconstpointer          a,
								gconstpointer          b);

void                  kraken_bookmark_set_scroll_pos         (KrakenBookmark      *bookmark,
								const char            *uri);
char *                kraken_bookmark_get_scroll_pos         (KrakenBookmark      *bookmark);


/* Helper functions for displaying bookmarks */
GtkWidget *           kraken_bookmark_menu_item_new          (KrakenBookmark      *bookmark);

void                  kraken_bookmark_connect                (KrakenBookmark *bookmark);

/* Bookmark metadata struct functions */

KrakenBookmarkMetadata *kraken_bookmark_get_updated_metadata   (KrakenBookmark  *bookmark);
KrakenBookmarkMetadata *kraken_bookmark_get_current_metadata   (KrakenBookmark  *bookmark);
gboolean              kraken_bookmark_metadata_compare       (KrakenBookmarkMetadata *d1,
                                                            KrakenBookmarkMetadata *d2);
KrakenBookmarkMetadata *kraken_bookmark_metadata_new           (void);
KrakenBookmarkMetadata *kraken_bookmark_metadata_copy          (KrakenBookmarkMetadata *meta);
void                  kraken_bookmark_metadata_free          (KrakenBookmarkMetadata *metadata);

#endif /* KRAKEN_BOOKMARK_H */
