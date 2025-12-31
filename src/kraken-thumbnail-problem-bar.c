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

#include "config.h"

#include <glib/gi18n.h>
#include <gtk/gtk.h>

#include "kraken-thumbnail-problem-bar.h"
#include "kraken-application.h"

#include "kraken-view.h"
#include <libkraken-private/kraken-file.h>

#define KRAKEN_THUMBNAIL_PROBLEM_BAR_GET_PRIVATE(o)\
	(G_TYPE_INSTANCE_GET_PRIVATE ((o), KRAKEN_TYPE_THUMBNAIL_PROBLEM_BAR, KrakenThumbnailProblemBarPrivate))

enum {
	PROP_VIEW = 1,
	NUM_PROPERTIES
};

enum {
    FIX_CACHE = 1,
    DISMISS
};

struct KrakenThumbnailProblemBarPrivate
{
	KrakenView *view;
	gulong selection_handler_id;
};

G_DEFINE_TYPE (KrakenThumbnailProblemBar, kraken_thumbnail_problem_bar, GTK_TYPE_INFO_BAR);

static void
thumbnail_problem_bar_response_cb (GtkInfoBar *infobar,
                                          gint  response_id,
                                      gpointer  user_data)
{
    KrakenThumbnailProblemBar *bar;

    bar = KRAKEN_THUMBNAIL_PROBLEM_BAR (infobar);

    switch (response_id) {
        case FIX_CACHE:
            g_spawn_command_line_sync ("sh -c \"pkexec kraken --fix-cache\"", NULL, NULL, NULL, NULL);
            kraken_application_check_thumbnail_cache (kraken_application_get_singleton ());
            kraken_window_slot_queue_reload (kraken_view_get_kraken_window_slot (bar->priv->view), FALSE);
            kraken_window_slot_check_bad_cache_bar (kraken_view_get_kraken_window_slot (bar->priv->view));
            break;
        case DISMISS:
            kraken_application_clear_cache_flag (kraken_application_get_singleton ());
            kraken_application_ignore_cache_problem (kraken_application_get_singleton ());
            gtk_widget_hide (GTK_WIDGET (infobar));
            break;
        default:
            break;
    }
}

static void
kraken_thumbnail_problem_bar_set_property (GObject      *object,
				 guint         prop_id,
				 const GValue *value,
				 GParamSpec   *pspec)
{
	KrakenThumbnailProblemBar *bar;

	bar = KRAKEN_THUMBNAIL_PROBLEM_BAR (object);

	switch (prop_id) {
    	case PROP_VIEW:
    		bar->priv->view = g_value_get_object (value);
    		break;
    	default:
    		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    		break;
    }
}

static void
kraken_thumbnail_problem_bar_constructed (GObject *obj)
{
    G_OBJECT_CLASS (kraken_thumbnail_problem_bar_parent_class)->constructed (obj);

    KrakenThumbnailProblemBar *bar = KRAKEN_THUMBNAIL_PROBLEM_BAR (obj);

    GtkWidget *content_area, *action_area;
    G_GNUC_UNUSED GtkWidget *w;
    GtkWidget *label;

    content_area = gtk_info_bar_get_content_area (GTK_INFO_BAR (bar));
    action_area = gtk_info_bar_get_action_area (GTK_INFO_BAR (bar));

    gtk_orientable_set_orientation (GTK_ORIENTABLE (action_area),
                                    GTK_ORIENTATION_HORIZONTAL);

    label = gtk_label_new (_("A problem has been detected with your thumbnail cache.  Fixing it will require administrative privileges."));

    /* w is useless - this method creates the widget and adds/refs it to the info bar at the same time */
    w = gtk_info_bar_add_button (GTK_INFO_BAR (bar),
                                 _("Fix now"),
                                 FIX_CACHE);
    w = gtk_info_bar_add_button (GTK_INFO_BAR (bar),
                                 _("Dismiss"),
                                 DISMISS);

    gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);
    gtk_style_context_add_class (gtk_widget_get_style_context (label),
                     "kraken-cluebar-label");
    gtk_widget_show (label);
    gtk_container_add (GTK_CONTAINER (content_area), label);

    g_signal_connect (bar, "response",
              G_CALLBACK (thumbnail_problem_bar_response_cb), bar);
}

static void
kraken_thumbnail_problem_bar_class_init (KrakenThumbnailProblemBarClass *klass)
{
	GObjectClass *object_class;

	object_class = G_OBJECT_CLASS (klass);

	object_class->set_property = kraken_thumbnail_problem_bar_set_property;
    object_class->constructed = kraken_thumbnail_problem_bar_constructed;

	g_object_class_install_property (object_class,
					 PROP_VIEW,
					 g_param_spec_object ("view",
							      "view",
							      "the KrakenView",
							      KRAKEN_TYPE_VIEW,
							      G_PARAM_WRITABLE |
							      G_PARAM_CONSTRUCT_ONLY |
							      G_PARAM_STATIC_STRINGS));

	g_type_class_add_private (klass, sizeof (KrakenThumbnailProblemBarPrivate));
}

static void
kraken_thumbnail_problem_bar_init (KrakenThumbnailProblemBar *bar)
{
    bar->priv = KRAKEN_THUMBNAIL_PROBLEM_BAR_GET_PRIVATE (bar);
}

GtkWidget *
kraken_thumbnail_problem_bar_new (KrakenView *view)
{
return g_object_new (KRAKEN_TYPE_THUMBNAIL_PROBLEM_BAR,
                     "view", view,
                     NULL);
}
