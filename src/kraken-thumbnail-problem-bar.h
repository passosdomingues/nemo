/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street - Suite 500, Boston, MA 02110-1335, USA.
 *
 */

#ifndef __KRAKEN_THUMBNAIL_PROBLEM_BAR_H
#define __KRAKEN_THUMBNAIL_PROBLEM_BAR_H

#include "kraken-view.h"

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define KRAKEN_TYPE_THUMBNAIL_PROBLEM_BAR         (kraken_thumbnail_problem_bar_get_type ())
#define KRAKEN_THUMBNAIL_PROBLEM_BAR(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), KRAKEN_TYPE_THUMBNAIL_PROBLEM_BAR, KrakenThumbnailProblemBar))
#define KRAKEN_THUMBNAIL_PROBLEM_BAR_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), KRAKEN_TYPE_THUMBNAIL_PROBLEM_BAR, KrakenThumbnailProblemBarClass))
#define KRAKEN_IS_THUMBNAIL_PROBLEM_BAR(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), KRAKEN_TYPE_THUMBNAIL_PROBLEM_BAR))
#define KRAKEN_IS_THUMBNAIL_PROBLEM_BAR_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), KRAKEN_TYPE_THUMBNAIL_PROBLEM_BAR))
#define KRAKEN_THUMBNAIL_PROBLEM_BAR_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), KRAKEN_TYPE_THUMBNAIL_PROBLEM_BAR, KrakenThumbnailProblemBarClass))

typedef struct KrakenThumbnailProblemBarPrivate KrakenThumbnailProblemBarPrivate;

typedef struct
{
	GtkInfoBar parent;

	KrakenThumbnailProblemBarPrivate *priv;
} KrakenThumbnailProblemBar;

typedef struct
{
	GtkInfoBarClass parent_class;
} KrakenThumbnailProblemBarClass;

GType		 kraken_thumbnail_problem_bar_get_type	(void) G_GNUC_CONST;
GtkWidget       *kraken_thumbnail_problem_bar_new         (KrakenView *view);

G_END_DECLS

#endif /* __KRAKEN_THUMBNAIL_PROBLEM_BAR_H */
