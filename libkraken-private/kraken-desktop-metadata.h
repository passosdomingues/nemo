/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * Kraken
 *
 * Copyright (C) 2011 Red Hat, Inc.
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
 * Authors: Cosimo Cecchi <cosimoc@redhat.com>
 */

#ifndef __KRAKEN_DESKTOP_METADATA_H__
#define __KRAKEN_DESKTOP_METADATA_H__

#include <glib.h>

#include <libkraken-private/kraken-file.h>

void kraken_desktop_metadata_init (void);
void kraken_desktop_set_metadata_string (KrakenFile *file,
                                           const gchar *name,
                                           const gchar *key,
                                           const gchar *string);

void kraken_desktop_set_metadata_stringv (KrakenFile *file,
                                            const char *name,
                                            const char *key,
                                            const char * const *stringv);

void kraken_desktop_clear_metadata       (KrakenFile *file);

gchar *kraken_desktop_get_metadata_string (KrakenFile *file,
                                         const gchar *name,
                                         const gchar *key);

gchar **kraken_desktop_get_metadata_stringv (KrakenFile *file,
                                           const char *name,
                                           const char *key);

gboolean kraken_desktop_update_metadata_from_keyfile (KrakenFile *file,
                                                        const gchar *name);

#endif /* __KRAKEN_DESKTOP_METADATA_H__ */
