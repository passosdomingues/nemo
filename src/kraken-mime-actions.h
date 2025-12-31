/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* kraken-mime-actions.h - uri-specific versions of mime action functions

   Copyright (C) 2000 Eazel, Inc.

   The Gnome Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The Gnome Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 51 Franklin Street - Suite 500,
   Boston, MA 02110-1335, USA.

   Authors: Maciej Stachowiak <mjs@eazel.com>
*/

#ifndef KRAKEN_MIME_ACTIONS_H
#define KRAKEN_MIME_ACTIONS_H

#include <gio/gio.h>

#include <libkraken-private/kraken-file.h>

#include "kraken-window.h"

KrakenFileAttributes kraken_mime_actions_get_required_file_attributes (void);

GAppInfo *             kraken_mime_get_default_application_for_file     (KrakenFile            *file);
GList *                kraken_mime_get_applications_for_file            (KrakenFile            *file);

GAppInfo *             kraken_mime_get_default_application_for_files    (GList                   *files);
GList *                kraken_mime_get_applications_for_files           (GList                   *file);

gboolean               kraken_mime_file_opens_in_view                   (KrakenFile            *file);
gboolean               kraken_mime_file_opens_in_external_app           (KrakenFile            *file);
void                   kraken_mime_activate_files                       (GtkWindow               *parent_window,
									   KrakenWindowSlot      *slot,
									   GList                   *files,
									   const char              *launch_directory,
									   KrakenWindowOpenFlags  flags,
									   gboolean                 user_confirmation);
void                   kraken_mime_activate_file                        (GtkWindow               *parent_window,
									   KrakenWindowSlot      *slot_info,
									   KrakenFile            *file,
									   const char              *launch_directory,
									   KrakenWindowOpenFlags  flags);
void                   kraken_mime_launch_fm_and_select_file            (GFile *file);

#endif /* KRAKEN_MIME_ACTIONS_H */
