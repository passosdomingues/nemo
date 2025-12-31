/*
 * kraken-previewer: kraken previewer DBus wrapper
 *
 * Copyright (C) 2011, Red Hat, Inc.
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

#include "kraken-previewer.h"

#define DEBUG_FLAG KRAKEN_DEBUG_PREVIEWER
#include <libkraken-private/kraken-debug.h>

#include <gio/gio.h>

G_DEFINE_TYPE (KrakenPreviewer, kraken_previewer, G_TYPE_OBJECT);

#define PREVIEWER_DBUS_NAME "org.kraken.Preview"
#define PREVIEWER_DBUS_IFACE "org.kraken.Preview"
#define PREVIEWER_DBUS_PATH "/org/kraken/Preview"

static KrakenPreviewer *singleton = NULL;

struct _KrakenPreviewerPriv {
  GDBusConnection *connection;
};

static void
kraken_previewer_dispose (GObject *object)
{
  KrakenPreviewer *self = KRAKEN_PREVIEWER (object);

  DEBUG ("%p", self);

  g_clear_object (&self->priv->connection);

  G_OBJECT_CLASS (kraken_previewer_parent_class)->dispose (object);
}

static GObject *
kraken_previewer_constructor (GType type,
                                guint n_construct_params,
                                GObjectConstructParam *construct_params)
{
  GObject *retval;

  if (singleton != NULL)
    return G_OBJECT (singleton);

  retval = G_OBJECT_CLASS (kraken_previewer_parent_class)->constructor
    (type, n_construct_params, construct_params);

  singleton = KRAKEN_PREVIEWER (retval);
  g_object_add_weak_pointer (retval, (gpointer) &singleton);

  return retval;
}

static void
kraken_previewer_init (KrakenPreviewer *self)
{
  GError *error = NULL;

  self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, KRAKEN_TYPE_PREVIEWER,
                                            KrakenPreviewerPriv);

  self->priv->connection = g_bus_get_sync (G_BUS_TYPE_SESSION,
                                           NULL, &error);

  if (error != NULL) {
    g_printerr ("Unable to initialize DBus connection: %s", error->message);
    g_error_free (error);
    return;
  }
}

static void
kraken_previewer_class_init (KrakenPreviewerClass *klass)
{
  GObjectClass *oclass;

  oclass = G_OBJECT_CLASS (klass);
  oclass->constructor = kraken_previewer_constructor;
  oclass->dispose = kraken_previewer_dispose;

  g_type_class_add_private (klass, sizeof (KrakenPreviewerPriv));
}

static void
previewer_show_file_ready_cb (GObject *source,
                              GAsyncResult *res,
                              gpointer user_data)
{
  KrakenPreviewer *self = user_data;
  GError *error = NULL;

  g_dbus_connection_call_finish (self->priv->connection,
                                 res, &error);

  if (error != NULL) {
    DEBUG ("Unable to call ShowFile on KrakenPreviewer: %s",
           error->message);
    g_error_free (error);
  }

  g_object_unref (self);
}

static void
previewer_close_ready_cb (GObject *source,
                          GAsyncResult *res,
                          gpointer user_data)
{
  KrakenPreviewer *self = user_data;
  GError *error = NULL;

  g_dbus_connection_call_finish (self->priv->connection,
                                 res, &error);

  if (error != NULL) {
    DEBUG ("Unable to call Close on KrakenPreviewer: %s",
           error->message);
    g_error_free (error);
  }

  g_object_unref (self);
}

KrakenPreviewer *
kraken_previewer_get_singleton (void)
{
  return g_object_new (KRAKEN_TYPE_PREVIEWER, NULL);
}

void
kraken_previewer_call_show_file (KrakenPreviewer *self,
                                   const gchar *uri,
                                   guint xid,
				   gboolean close_if_already_visible)
{
  if (self->priv->connection == NULL) {
    g_printerr ("No DBus connection available");
    return;
  }

  g_dbus_connection_call (self->priv->connection,
                          PREVIEWER_DBUS_NAME,
                          PREVIEWER_DBUS_PATH,
                          PREVIEWER_DBUS_IFACE,
                          "ShowFile",
                          g_variant_new ("(sib)",
                            uri, xid, close_if_already_visible),
                          NULL,
                          G_DBUS_CALL_FLAGS_NONE,
                          -1,
                          NULL,
                          previewer_show_file_ready_cb,
                          g_object_ref (self));
}

void
kraken_previewer_call_close (KrakenPreviewer *self)
{
  if (self->priv->connection == NULL) {
    g_printerr ("No DBus connection available");
    return;
  }

  /* don't autostart the previewer if it's not running */
  g_dbus_connection_call (self->priv->connection,
                          PREVIEWER_DBUS_NAME,
                          PREVIEWER_DBUS_PATH,
                          PREVIEWER_DBUS_IFACE,
                          "Close",
                          NULL,
                          NULL,
                          G_DBUS_CALL_FLAGS_NO_AUTO_START,
                          -1,
                          NULL,
                          previewer_close_ready_cb,
                          g_object_ref (self));
}
