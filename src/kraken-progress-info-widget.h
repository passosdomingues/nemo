/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * kraken-progress-info-widget.h: file operation progress user interface.
 *
 * Copyright (C) 2007, 2011 Red Hat, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street - Suite 500,
 * Boston, MA 02110-1335, USA.
 *
 * Authors: Alexander Larsson <alexl@redhat.com>
 *          Cosimo Cecchi <cosimoc@redhat.com>
 *
 */

#ifndef __KRAKEN_PROGRESS_INFO_WIDGET_H__
#define __KRAKEN_PROGRESS_INFO_WIDGET_H__

#include <gtk/gtk.h>

#include <libkraken-private/kraken-progress-info.h>

#define KRAKEN_TYPE_PROGRESS_INFO_WIDGET kraken_progress_info_widget_get_type()
#define KRAKEN_PROGRESS_INFO_WIDGET(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_PROGRESS_INFO_WIDGET, KrakenProgressInfoWidget))
#define KRAKEN_PROGRESS_INFO_WIDGET_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_PROGRESS_INFO_WIDGET, KrakenProgressInfoWidgetClass))
#define KRAKEN_IS_PROGRESS_INFO_WIDGET(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_PROGRESS_INFO_WIDGET))
#define KRAKEN_IS_PROGRESS_INFO_WIDGET_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_PROGRESS_INFO_WIDGET))
#define KRAKEN_PROGRESS_INFO_WIDGET_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_PROGRESS_INFO_WIDGET, KrakenProgressInfoWidgetClass))

typedef struct _KrakenProgressInfoWidgetPriv KrakenProgressInfoWidgetPriv;

typedef struct {
	GtkBox parent;

	/* private */
	KrakenProgressInfoWidgetPriv *priv;
} KrakenProgressInfoWidget;

typedef struct {
	GtkBoxClass parent_class;
} KrakenProgressInfoWidgetClass;

struct _KrakenProgressInfoWidgetPriv {
    KrakenProgressInfo *info;

    GtkWidget *stack;
    GtkWidget *separator;

    /* pre-start page */
    GtkWidget *pre_info; /* GtkLabel */

    GtkWidget *status; /* GtkLabel */
    GtkWidget *details; /* GtkLabel */
    GtkWidget *progress_bar;
};

GType kraken_progress_info_widget_get_type (void);

GtkWidget * kraken_progress_info_widget_new (KrakenProgressInfo *info);

#endif /* __KRAKEN_PROGRESS_INFO_WIDGET_H__ */
