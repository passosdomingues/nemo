/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
   kraken-job-queue.h - file operation queue

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
*/

#ifndef __KRAKEN_JOB_QUEUE_H__
#define __KRAKEN_JOB_QUEUE_H__

#include <glib-object.h>

#include <libkraken-private/kraken-progress-info.h>

#define KRAKEN_TYPE_JOB_QUEUE kraken_job_queue_get_type()
#define KRAKEN_JOB_QUEUE(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_JOB_QUEUE, KrakenJobQueue))
#define KRAKEN_JOB_QUEUE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_JOB_QUEUE, KrakenJobQueueClass))
#define KRAKEN_IS_JOB_QUEUE(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_JOB_QUEUE))
#define KRAKEN_IS_JOB_QUEUE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_JOB_QUEUE))
#define KRAKEN_JOB_QUEUE_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_JOB_QUEUE, KrakenJobQueueClass))

typedef struct _KrakenJobQueue KrakenJobQueue;
typedef struct _KrakenJobQueueClass KrakenJobQueueClass;
typedef struct _KrakenJobQueuePriv KrakenJobQueuePriv;

struct _KrakenJobQueue {
  GObject parent;

  /* private */
  KrakenJobQueuePriv *priv;
};

struct _KrakenJobQueueClass {
  GObjectClass parent_class;
};

GType kraken_job_queue_get_type (void);

KrakenJobQueue *kraken_job_queue_get (void);

void kraken_job_queue_add_new_job (KrakenJobQueue *self,
                                 GIOSchedulerJobFunc job_func,
                                 gpointer user_data,
                                 GCancellable *cancellable,
                                 KrakenProgressInfo *info,
                                 gboolean start_immediately);

void kraken_job_queue_start_next_job (KrakenJobQueue *self);

void kraken_job_queue_start_job_by_info (KrakenJobQueue     *self,
                                       KrakenProgressInfo *info);

GList *kraken_job_queue_get_all_jobs (KrakenJobQueue *self);

G_END_DECLS

#endif /* __KRAKEN_JOB_QUEUE_H__ */
