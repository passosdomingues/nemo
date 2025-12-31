/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   kraken-link.h: .

   Copyright (C) 2001 Red Hat, Inc.

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
  
   Authors: Jonathan Blandford <jrb@redhat.com>
*/

#ifndef KRAKEN_LINK_H
#define KRAKEN_LINK_H

#include <gdk/gdk.h>

gboolean         kraken_link_local_create                      (const char        *directory_uri,
								  const char        *base_name,
								  const char        *display_name,
								  const char        *image,
								  const char        *target_uri,
								  const GdkPoint    *point,
								  int                monitor,
								  gboolean           unique_filename);
gboolean         kraken_link_local_set_text                    (const char        *uri,
								 const char        *text);
gboolean         kraken_link_local_set_icon                    (const char        *uri,
								  const char        *icon);
char *           kraken_link_local_get_text                    (const char        *uri);
char *           kraken_link_local_get_link_uri                (const char        *uri);
void             kraken_link_get_link_info_given_file_contents (const char        *file_contents,
								  int                link_file_size,
								  const char        *file_uri,
								  char             **uri,
								  char             **name,
								  GIcon            **icon,
								  gboolean          *is_launcher,
								  gboolean          *is_foreign);

#endif /* KRAKEN_LINK_H */
