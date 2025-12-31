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
 * Author: Cosimo Cecchi <cosimoc@redhat.com>
 */

#ifndef __KRAKEN_PROGRESS_INFO_MANAGER_H__
#define __KRAKEN_PROGRESS_INFO_MANAGER_H__

#include <glib-object.h>

#include <libkraken-private/kraken-progress-info.h>

#define KRAKEN_TYPE_PROGRESS_INFO_MANAGER kraken_progress_info_manager_get_type()
#define KRAKEN_PROGRESS_INFO_MANAGER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_PROGRESS_INFO_MANAGER, KrakenProgressInfoManager))
#define KRAKEN_PROGRESS_INFO_MANAGER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_PROGRESS_INFO_MANAGER, KrakenProgressInfoManagerClass))
#define KRAKEN_IS_PROGRESS_INFO_MANAGER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_PROGRESS_INFO_MANAGER))
#define KRAKEN_IS_PROGRESS_INFO_MANAGER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_PROGRESS_INFO_MANAGER))
#define KRAKEN_PROGRESS_INFO_MANAGER_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_PROGRESS_INFO_MANAGER, KrakenProgressInfoManagerClass))

typedef struct _KrakenProgressInfoManager KrakenProgressInfoManager;
typedef struct _KrakenProgressInfoManagerClass KrakenProgressInfoManagerClass;
typedef struct _KrakenProgressInfoManagerPriv KrakenProgressInfoManagerPriv;

struct _KrakenProgressInfoManager {
  GObject parent;

  /* private */
  KrakenProgressInfoManagerPriv *priv;
};

struct _KrakenProgressInfoManagerClass {
  GObjectClass parent_class;
};

GType kraken_progress_info_manager_get_type (void);

KrakenProgressInfoManager* kraken_progress_info_manager_new (void);

void kraken_progress_info_manager_add_new_info (KrakenProgressInfoManager *self,
                                                  KrakenProgressInfo *info);
GList *kraken_progress_info_manager_get_all_infos (KrakenProgressInfoManager *self);

G_END_DECLS

#endif /* __KRAKEN_PROGRESS_INFO_MANAGER_H__ */
