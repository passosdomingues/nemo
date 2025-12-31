/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* fm-error-reporting.h - interface for file manager functions that report
 			  errors to the user.

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

   Authors: John Sullivan <sullivan@eazel.com>
*/

#ifndef KRAKEN_ERROR_REPORTING_H
#define KRAKEN_ERROR_REPORTING_H

#include <gtk/gtk.h>
#include <libkraken-private/kraken-file.h>

void kraken_report_error_loading_directory	 (KrakenFile   *file,
						  GError         *error,
						  GtkWindow	 *parent_window);
void kraken_report_error_renaming_file         (KrakenFile *file,
						  const char *new_name,
						  GError *error,
						  GtkWindow *parent_window);
void kraken_report_error_setting_permissions (KrakenFile   *file,
						GError         *error,
						GtkWindow	 *parent_window);
void kraken_report_error_setting_owner       (KrakenFile   *file,
						GError         *error,  
						GtkWindow	 *parent_window);
void kraken_report_error_setting_group       (KrakenFile   *file,
						GError         *error,
						GtkWindow	 *parent_window);

/* FIXME bugzilla.gnome.org 42394: Should this file be renamed or should this function be moved? */
void kraken_rename_file                      (KrakenFile   *file,
						const char     *new_name,
						KrakenFileOperationCallback callback,
						gpointer callback_data);

#endif /* KRAKEN_ERROR_REPORTING_H */
