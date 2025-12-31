/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   kraken-progress-info.h: file operation progress info.
 
   Copyright (C) 2007 Red Hat, Inc.
  
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
  
   Author: Alexander Larsson <alexl@redhat.com>
*/

#ifndef KRAKEN_PROGRESS_INFO_H
#define KRAKEN_PROGRESS_INFO_H

#include <glib-object.h>
#include <gio/gio.h>

#define KRAKEN_TYPE_PROGRESS_INFO         (kraken_progress_info_get_type ())
#define KRAKEN_PROGRESS_INFO(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), KRAKEN_TYPE_PROGRESS_INFO, KrakenProgressInfo))
#define KRAKEN_PROGRESS_INFO_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), KRAKEN_TYPE_PROGRESS_INFO, KrakenProgressInfoClass))
#define KRAKEN_IS_PROGRESS_INFO(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), KRAKEN_TYPE_PROGRESS_INFO))
#define KRAKEN_IS_PROGRESS_INFO_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), KRAKEN_TYPE_PROGRESS_INFO))
#define KRAKEN_PROGRESS_INFO_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), KRAKEN_TYPE_PROGRESS_INFO, KrakenProgressInfoClass))

typedef struct _KrakenProgressInfo      KrakenProgressInfo;
typedef struct _KrakenProgressInfoClass KrakenProgressInfoClass;

GType kraken_progress_info_get_type (void) G_GNUC_CONST;

/* Signals:
   "changed" - status or details changed
   "progress-changed" - the percentage progress changed (or we pulsed if in activity_mode
   "started" - emited on job start
   "finished" - emitted when job is done
   
   All signals are emitted from idles in main loop.
   All methods are threadsafe.
 */

KrakenProgressInfo *kraken_progress_info_new (void);

GList *       kraken_get_all_progress_info (void);

char *        kraken_progress_info_get_status      (KrakenProgressInfo *info);
char *        kraken_progress_info_get_details     (KrakenProgressInfo *info);
char *        kraken_progress_info_get_initial_details (KrakenProgressInfo *info);
double        kraken_progress_info_get_progress    (KrakenProgressInfo *info);
GCancellable *kraken_progress_info_get_cancellable (KrakenProgressInfo *info);
void          kraken_progress_info_cancel          (KrakenProgressInfo *info);
gboolean      kraken_progress_info_get_is_started  (KrakenProgressInfo *info);
gboolean      kraken_progress_info_get_is_finished (KrakenProgressInfo *info);
gboolean      kraken_progress_info_get_is_paused   (KrakenProgressInfo *info);

void          kraken_progress_info_queue           (KrakenProgressInfo *info);
void          kraken_progress_info_start           (KrakenProgressInfo *info);
void          kraken_progress_info_finish          (KrakenProgressInfo *info);
void          kraken_progress_info_pause           (KrakenProgressInfo *info);
void          kraken_progress_info_resume          (KrakenProgressInfo *info);
void          kraken_progress_info_set_status      (KrakenProgressInfo *info,
						      const char           *status);
void          kraken_progress_info_take_status     (KrakenProgressInfo *info,
						      char                 *status);
void          kraken_progress_info_set_details     (KrakenProgressInfo *info,
						      const char           *details);
void          kraken_progress_info_take_initial_details (KrakenProgressInfo *info,
                              char                 *initial_details);
void          kraken_progress_info_take_details    (KrakenProgressInfo *info,
						      char                 *details);
void          kraken_progress_info_set_progress    (KrakenProgressInfo *info,
						      double                current,
						      double                total);
void          kraken_progress_info_pulse_progress  (KrakenProgressInfo *info);

gdouble       kraken_progress_info_get_elapsed_time (KrakenProgressInfo *info);


#endif /* KRAKEN_PROGRESS_INFO_H */
