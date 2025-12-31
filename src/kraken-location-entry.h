/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * Kraken
 *
 * Copyright (C) 2000 Eazel, Inc.
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
 * Author: Maciej Stachowiak <mjs@eazel.com>
 *         Ettore Perazzoli <ettore@gnu.org>
 */

#ifndef KRAKEN_LOCATION_ENTRY_H
#define KRAKEN_LOCATION_ENTRY_H

#include <libkraken-private/kraken-entry.h>

#define KRAKEN_TYPE_LOCATION_ENTRY kraken_location_entry_get_type()
#define KRAKEN_LOCATION_ENTRY(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), KRAKEN_TYPE_LOCATION_ENTRY, KrakenLocationEntry))
#define KRAKEN_LOCATION_ENTRY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), KRAKEN_TYPE_LOCATION_ENTRY, KrakenLocationEntryClass))
#define KRAKEN_IS_LOCATION_ENTRY(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), KRAKEN_TYPE_LOCATION_ENTRY))
#define KRAKEN_IS_LOCATION_ENTRY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), KRAKEN_TYPE_LOCATION_ENTRY))
#define KRAKEN_LOCATION_ENTRY_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), KRAKEN_TYPE_LOCATION_ENTRY, KrakenLocationEntryClass))

typedef struct KrakenLocationEntryDetails KrakenLocationEntryDetails;

typedef struct KrakenLocationEntry {
	KrakenEntry parent;
	KrakenLocationEntryDetails *details;
} KrakenLocationEntry;

typedef struct {
	KrakenEntryClass parent_class;
} KrakenLocationEntryClass;

typedef enum {
	KRAKEN_LOCATION_ENTRY_ACTION_GOTO,
	KRAKEN_LOCATION_ENTRY_ACTION_CLEAR
} KrakenLocationEntryAction;

GType      kraken_location_entry_get_type     	(void);
GtkWidget* kraken_location_entry_new          	(void);
void       kraken_location_entry_set_special_text     (KrakenLocationEntry *entry,
							 const char            *special_text);
void       kraken_location_entry_set_secondary_action (KrakenLocationEntry *entry,
							 KrakenLocationEntryAction secondary_action);
KrakenLocationEntryAction kraken_location_entry_get_secondary_action (KrakenLocationEntry *entry);
void       kraken_location_entry_update_current_location (KrakenLocationEntry *entry,
							    const char *path);

#endif /* KRAKEN_LOCATION_ENTRY_H */
