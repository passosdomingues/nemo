/*
 * kraken-dbus-manager: kraken DBus interface
 *
 * Copyright (C) 2010, Red Hat, Inc.
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Suite 500, MA 02110-1335, USA.
 *
 * Author: Cosimo Cecchi <cosimoc@redhat.com>
 *
 */

#ifndef __KRAKEN_DBUS_MANAGER_H__
#define __KRAKEN_DBUS_MANAGER_H__

#include <glib-object.h>
#include <gio/gio.h>

typedef struct _KrakenDBusManager KrakenDBusManager;
typedef struct _KrakenDBusManagerClass KrakenDBusManagerClass;

GType kraken_dbus_manager_get_type (void);
KrakenDBusManager * kraken_dbus_manager_new (void);

#endif /* __KRAKEN_DBUS_MANAGER_H__ */
