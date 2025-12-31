/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * Copyright (C) 2005 Novell, Inc.
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
 * You should have received a copy of the GNU General Public
 * License along with this program; see the file COPYING.  If not,
 * write to the Free Software Foundation, Inc., 51 Franklin Street - Suite 500,
 * Boston, MA 02110-1335, USA.
 *
 * Author: Anders Carlsson <andersca@imendio.com>
 *
 */

#include <config.h>
#include <string.h>

#include "kraken-query.h"
#include <eel/eel-glib-extensions.h>
#include <glib/gi18n.h>
#include <libkraken-private/kraken-file-utilities.h>

struct KrakenQueryDetails {
    gchar *file_pattern;
    gchar *content_pattern;
    char *location_uri;
    GList *mime_types;
    gboolean show_hidden;
    gboolean file_case_sensitive;
    gboolean file_use_regex;
    gboolean content_case_sensitive;
    gboolean content_use_regex;
    gboolean count_hits;
    gboolean recurse;
    gboolean semantic;
};

G_DEFINE_TYPE (KrakenQuery, kraken_query, G_TYPE_OBJECT);

static void
finalize (GObject *object)
{
	KrakenQuery *query;

	query = KRAKEN_QUERY (object);
    g_free (query->details->file_pattern);
	g_free (query->details->content_pattern);
	g_free (query->details->location_uri);

	G_OBJECT_CLASS (kraken_query_parent_class)->finalize (object);
}

static void
kraken_query_class_init (KrakenQueryClass *class)
{
	GObjectClass *gobject_class;

	gobject_class = G_OBJECT_CLASS (class);
	gobject_class->finalize = finalize;

	g_type_class_add_private (class, sizeof (KrakenQueryDetails));
}

static void
kraken_query_init (KrakenQuery *query)
{
	query->details = G_TYPE_INSTANCE_GET_PRIVATE (query, KRAKEN_TYPE_QUERY,
						      KrakenQueryDetails);
}

KrakenQuery *
kraken_query_new (void)
{
	return g_object_new (KRAKEN_TYPE_QUERY,  NULL);
}

char *
kraken_query_get_file_pattern (KrakenQuery *query)
{
    g_return_val_if_fail (KRAKEN_IS_QUERY (query), NULL);

	return g_strdup (query->details->file_pattern);
}

void
kraken_query_set_file_pattern (KrakenQuery *query, const char *text)
{
    g_return_if_fail (KRAKEN_IS_QUERY (query));

	g_free (query->details->file_pattern);
	query->details->file_pattern = g_strstrip (g_strdup (text));
}

char *
kraken_query_get_content_pattern (KrakenQuery *query)
{
    g_return_val_if_fail (KRAKEN_IS_QUERY (query), NULL);

    return g_strdup (query->details->content_pattern);
}

void
kraken_query_set_content_pattern (KrakenQuery *query, const char *text)
{
    g_return_if_fail (KRAKEN_IS_QUERY (query));

    g_clear_pointer (&query->details->content_pattern, g_free);

    if (text && text[0] != '\0') {
        query->details->content_pattern = g_strstrip (g_strdup (text));
    }
}

gboolean
kraken_query_has_content_pattern (KrakenQuery *query)
{
    g_return_val_if_fail (KRAKEN_IS_QUERY (query), FALSE);

    return query->details->content_pattern != NULL;
}

char *
kraken_query_get_location (KrakenQuery *query)
{
    g_return_val_if_fail (KRAKEN_IS_QUERY (query), NULL);

	return g_strdup (query->details->location_uri);
}

void
kraken_query_set_location (KrakenQuery *query, const char *uri)
{
    g_return_if_fail (KRAKEN_IS_QUERY (query));

	g_free (query->details->location_uri);
	query->details->location_uri = g_strdup (uri);
}

GList *
kraken_query_get_mime_types (KrakenQuery *query)
{
    g_return_val_if_fail (KRAKEN_IS_QUERY (query), NULL);

	return eel_g_str_list_copy (query->details->mime_types);
}

void
kraken_query_set_mime_types (KrakenQuery *query, GList *mime_types)
{
    g_return_if_fail (KRAKEN_IS_QUERY (query));

	g_list_free_full (query->details->mime_types, g_free);
	query->details->mime_types = eel_g_str_list_copy (mime_types);
}

void
kraken_query_add_mime_type (KrakenQuery *query, const char *mime_type)
{
    g_return_if_fail (KRAKEN_IS_QUERY (query));

	query->details->mime_types = g_list_append (query->details->mime_types,
						    g_strdup (mime_type));
}

void
kraken_query_set_show_hidden (KrakenQuery *query, gboolean hidden)
{
    g_return_if_fail (KRAKEN_IS_QUERY (query));

    query->details->show_hidden = hidden;
}

gboolean
kraken_query_get_show_hidden (KrakenQuery *query)
{
    g_return_val_if_fail (KRAKEN_IS_QUERY (query), FALSE);

    return query->details->show_hidden;
}

char *
kraken_query_to_readable_string (KrakenQuery *query)
{
    g_return_val_if_fail (KRAKEN_IS_QUERY (query), NULL);

    GFile *file;
    gchar *location_title, *readable;

	if (!query || !query->details->file_pattern || query->details->file_pattern[0] == '\0') {
		return g_strdup (_("Search"));
	}

    file = g_file_new_for_uri (query->details->location_uri);
    location_title = kraken_compute_search_title_for_location (file);

    g_object_unref (file);

    readable = g_strdup_printf (_("Search in \"%s\""), location_title);

    g_free (location_title);

    return readable;
}

gboolean
kraken_query_get_file_case_sensitive (KrakenQuery *query)
{
    g_return_val_if_fail (KRAKEN_IS_QUERY (query), FALSE);
    return query->details->file_case_sensitive;
}

void
kraken_query_set_file_case_sensitive (KrakenQuery *query, gboolean case_sensitive)
{
    g_return_if_fail (KRAKEN_IS_QUERY (query));

    query->details->file_case_sensitive = case_sensitive;
}

gboolean
kraken_query_get_content_case_sensitive (KrakenQuery *query)
{
    g_return_val_if_fail (KRAKEN_IS_QUERY (query), FALSE);
    return query->details->content_case_sensitive;
}

void
kraken_query_set_content_case_sensitive (KrakenQuery *query, gboolean case_sensitive)
{
    g_return_if_fail (KRAKEN_IS_QUERY (query));

    query->details->content_case_sensitive = case_sensitive;
}

gboolean
kraken_query_get_use_file_regex (KrakenQuery *query)
{
    g_return_val_if_fail (KRAKEN_IS_QUERY (query), FALSE);
    return query->details->file_use_regex;
}

void
kraken_query_set_use_file_regex (KrakenQuery *query, gboolean file_use_regex)
{
    g_return_if_fail (KRAKEN_IS_QUERY (query));

    query->details->file_use_regex = file_use_regex;
}

gboolean
kraken_query_get_use_content_regex (KrakenQuery *query)
{
    g_return_val_if_fail (KRAKEN_IS_QUERY (query), FALSE);
    return query->details->content_use_regex;
}

void
kraken_query_set_use_content_regex (KrakenQuery *query, gboolean content_use_regex)
{
    g_return_if_fail (KRAKEN_IS_QUERY (query));

    query->details->content_use_regex = content_use_regex;
}

gboolean
kraken_query_get_count_hits (KrakenQuery *query)
{
    g_return_val_if_fail (KRAKEN_IS_QUERY (query), FALSE);
    return query->details->count_hits;
}

void
kraken_query_set_count_hits (KrakenQuery *query, gboolean count_hits)
{
    g_return_if_fail (KRAKEN_IS_QUERY (query));

    query->details->count_hits = count_hits;
}

gboolean
kraken_query_get_recurse (KrakenQuery *query)
{
    g_return_val_if_fail (KRAKEN_IS_QUERY (query), FALSE);
    return query->details->recurse;
}

void
kraken_query_set_recurse (KrakenQuery *query, gboolean recurse)
{
    g_return_if_fail (KRAKEN_IS_QUERY (query));

    query->details->recurse = recurse;
}

gboolean
kraken_query_get_semantic (KrakenQuery *query)
{
    g_return_val_if_fail (KRAKEN_IS_QUERY (query), FALSE);
    return query->details->semantic;
}

void
kraken_query_set_semantic (KrakenQuery *query, gboolean semantic)
{
    g_return_if_fail (KRAKEN_IS_QUERY (query));

    query->details->semantic = semantic;
}

