/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * kraken-application: main Kraken application class.
 *
 * Copyright (C) 1999, 2000 Red Hat, Inc.
 * Copyright (C) 2000, 2001 Eazel, Inc.
 * Copyright (C) 2010, Cosimo Cecchi <cosimoc@gnome.org>
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
 * Authors: Elliot Lee <sopwith@redhat.com>,
 *          Darin Adler <darin@bentspoon.com>
 *          Cosimo Cecchi <cosimoc@gnome.org>
 *
 */

#include <config.h>

#include "kraken-desktop-application.h"

#include "kraken-desktop-icon-view.h"
#include "kraken-desktop-icon-grid-view.h"
#include "kraken-icon-view.h"
#include "kraken-list-view.h"
#include "kraken-freedesktop-dbus.h"

#include "kraken-desktop-manager.h"
#include "kraken-image-properties-page.h"
#include "kraken-previewer.h"
#include "kraken-progress-ui-handler.h"
#include "kraken-window-bookmarks.h"
#include "kraken-window-private.h"

#include <eel/eel-vfs-extensions.h>
#include <libkraken-private/kraken-desktop-link-monitor.h>
#include <libkraken-private/kraken-desktop-metadata.h>
#include <libkraken-private/kraken-file-utilities.h>
#include <libkraken-private/kraken-global-preferences.h>
#include <libkraken-private/kraken-module.h>
#include <libkraken-private/kraken-signaller.h>
#include <libkraken-private/kraken-undo-manager.h>
#include <libkraken-extension/kraken-menu-provider.h>

#define DEBUG_FLAG KRAKEN_DEBUG_APPLICATION
#include <libkraken-private/kraken-debug.h>

#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <glib/gstdio.h>
#include <glib/gi18n.h>
#include <gdk/gdkx.h>
#include <X11/Xatom.h>

G_DEFINE_TYPE (KrakenDesktopApplication, kraken_desktop_application, KRAKEN_TYPE_APPLICATION);

struct _KrakenDesktopApplicationPriv {
    KrakenDesktopManager *desktop_manager;
    KrakenFreedesktopDBus *fdb_manager;
};

static void
kraken_desktop_application_init (KrakenDesktopApplication *application)
{
    application->priv = G_TYPE_INSTANCE_GET_PRIVATE (application,
                                                     KRAKEN_TYPE_DESKTOP_APPLICATION,
                                                     KrakenDesktopApplicationPriv);
}

static void
kraken_desktop_application_finalize (GObject *object)
{
    KrakenDesktopApplication *app = KRAKEN_DESKTOP_APPLICATION (object);

    g_clear_object (&app->priv->fdb_manager);

    G_OBJECT_CLASS (kraken_desktop_application_parent_class)->finalize (object);
}

static void
init_desktop (KrakenDesktopApplication *self)
{
	/* Initialize the desktop link monitor singleton */
	kraken_desktop_link_monitor_get ();
    kraken_desktop_metadata_init ();

    self->priv->desktop_manager = kraken_desktop_manager_get ();
}

/* from libwnck/xutils.c, comes as LGPLv2+ */
static char *
latin1_to_utf8 (const char *latin1)
{
  GString *str;
  const char *p;

  str = g_string_new (NULL);

  p = latin1;
  while (*p)
    {
      g_string_append_unichar (str, (gunichar) *p);
      ++p;
    }

  return g_string_free (str, FALSE);
}

/* derived from libwnck/xutils.c, comes as LGPLv2+ */
static void
_get_wmclass (Display *xdisplay,
              Window   xwindow,
              char   **res_class,
              char   **res_name)
{
  XClassHint ch;

  ch.res_name = NULL;
  ch.res_class = NULL;

  gdk_error_trap_push ();
  XGetClassHint (xdisplay, xwindow, &ch);
  gdk_error_trap_pop_ignored ();

  if (res_class)
    *res_class = NULL;

  if (res_name)
    *res_name = NULL;

  if (ch.res_name)
    {
      if (res_name)
        *res_name = latin1_to_utf8 (ch.res_name);

      XFree (ch.res_name);
    }

  if (ch.res_class)
    {
      if (res_class)
        *res_class = latin1_to_utf8 (ch.res_class);

      XFree (ch.res_class);
    }
}

static gboolean
desktop_handler_is_ignored (GdkWindow *window, gchar **ignored)
{
    gboolean ret;
    Window xw;
    Display *xd;
    guint i;

    if (ignored == NULL) {
        return FALSE;
    }

    ret = FALSE;

    xw = gdk_x11_window_get_xid (GDK_X11_WINDOW (window));
    xd = gdk_x11_display_get_xdisplay (gdk_display_get_default ());

    for (i = 0; i < g_strv_length (ignored); i++) {
        gchar *wmclass, *wm_class_casefolded, *match_string_casefolded;

        _get_wmclass (xd, xw, &wmclass, NULL);

        if (wmclass != NULL) {
            wm_class_casefolded = g_utf8_casefold (wmclass, -1);
            match_string_casefolded = g_utf8_casefold (ignored[i], -1);

            if (g_strstr_len (wm_class_casefolded, -1, match_string_casefolded) != NULL) {
                ret = TRUE;
            }

            g_free (wm_class_casefolded);
            g_free (match_string_casefolded);

            g_free (wmclass);
        }

        if (ret == TRUE)
            break;
    }

    return ret;
}

static gboolean
desktop_already_managed (void)
{
    GdkScreen *screen;
    GList *windows, *iter;
    gboolean ret;

    screen = gdk_screen_get_default ();

    windows = gdk_screen_get_window_stack (screen);

    ret = FALSE;

    for (iter = windows; iter != NULL; iter = iter->next) {
        GdkWindow *window = GDK_WINDOW (iter->data);

        if (gdk_window_get_type_hint (window) == GDK_WINDOW_TYPE_HINT_DESKTOP) {
            GSettings *desktop_preferences = g_settings_new("org.kraken.desktop");

            gchar **ignored = g_settings_get_strv (desktop_preferences, KRAKEN_PREFERENCES_DESKTOP_IGNORED_DESKTOP_HANDLERS);
            if (!desktop_handler_is_ignored (window, ignored)) {
                ret = TRUE;
            }
            g_strfreev (ignored);
            g_object_unref (desktop_preferences);

            break;
        }
    }

    g_list_free_full (windows, (GDestroyNotify) g_object_unref);

    if (ret) {
        g_warning ("Desktop already managed by another application, skipping desktop setup.\n"
                   "To change this, modify org.kraken.desktop 'ignored-desktop-handlers'.\n");
    }

    return ret;
}

static gboolean
kraken_desktop_application_local_command_line (GApplication *application,
                                             gchar      ***arguments,
                                             gint         *exit_status)
{
    GFile **files;
    gint len;

    gboolean version = FALSE;
    gboolean kill_shell = FALSE;
    gboolean debug = FALSE;

    const GOptionEntry options[] = {
        { "version", '\0', 0, G_OPTION_ARG_NONE, &version,
          N_("Show the version of the program."), NULL },
        { "debug", 0, 0, G_OPTION_ARG_NONE, &debug,
          "Enable debugging code.  Example usage: 'KRAKEN_DEBUG=Desktop,Actions kraken-desktop --debug'.  Use KRAKEN_DEBUG=all for more topics.", NULL },
        { "quit", 'q', 0, G_OPTION_ARG_NONE, &kill_shell, 
          N_("Quit Kraken Desktop."), NULL },
        { NULL }
    };

    GOptionContext *context;
    GError *error = NULL;
    gint argc = 0;
    gchar **argv = NULL;

    *exit_status = EXIT_SUCCESS;

    context = g_option_context_new (_("\n\nManage the desktop with the file manager"));
    g_option_context_add_main_entries (context, options, NULL);
    g_option_context_add_group (context, gtk_get_option_group (TRUE));

    argv = *arguments;
    argc = g_strv_length (argv);

    if (!g_option_context_parse (context, &argc, &argv, &error)) {
        g_printerr ("Could not parse arguments: %s\n", error->message);
        g_error_free (error);

        *exit_status = EXIT_FAILURE;
        goto out;
    }

    if (version) {
        g_print ("kraken-desktop " VERSION "\n");
        goto out;
    }

    if (debug) {
#if (GLIB_CHECK_VERSION(2,80,0))
        const gchar* const domains[] = { "Kraken", NULL };
        g_log_writer_default_set_debug_domains (domains);
#else
        g_setenv ("G_MESSAGES_DEBUG", "all", TRUE);
#endif
    }

    if (kraken_user_is_root () && !kraken_treating_root_as_normal ()) {
        g_printerr ("kraken-desktop cannot be run as root, please try again as a normal user.\n"
                    "Check 'man kraken' to see how to change this behavior.");
        goto out;
    }

    g_application_register (application, NULL, &error);

    if (error != NULL) {
        g_printerr ("Could not register the application: %s\n", error->message);
        g_error_free (error);

        *exit_status = EXIT_FAILURE;
        goto out;
    }

    if (kill_shell) {
        g_printerr ("Killing kraken-desktop, as requested\n");
        g_action_group_activate_action (G_ACTION_GROUP (application),
                        "quit", NULL);
        goto out;
    }

    if (desktop_already_managed ()) {
        goto out;
    }

    files = g_malloc0 (2 * sizeof (GFile *));
    len = 1;
    files[0] = g_file_new_for_uri (EEL_DESKTOP_URI);
    files[1] = NULL;

    g_application_open (application, files, len, "");

    g_object_unref (files[0]);
    g_free (files);

 out:
    g_option_context_free (context);

    return TRUE;
}

static void
kraken_desktop_application_continue_startup (KrakenApplication *app)
{
    /* Check the user's Desktop and config directories and post warnings
     * if there are problems.
     */
    kraken_application_check_required_directory (app, kraken_get_desktop_directory ());
    kraken_application_check_required_directory (app, kraken_get_user_directory ());

    KRAKEN_DESKTOP_APPLICATION (app)->priv->fdb_manager = kraken_freedesktop_dbus_new ();

	/* register views */
	kraken_desktop_icon_view_register ();
    kraken_desktop_icon_grid_view_register ();
    kraken_icon_view_register ();
}

static void
kraken_desktop_application_continue_quit (KrakenApplication *app)
{
    KrakenDesktopApplication *self = KRAKEN_DESKTOP_APPLICATION (app);

    g_clear_object (&self->priv->desktop_manager);
}

static void
kraken_desktop_application_open (GApplication *app,
                               GFile **files,
                               gint n_files,
                               const gchar *geometry)
{
    KrakenDesktopApplication *self = KRAKEN_DESKTOP_APPLICATION (app);

    DEBUG ("Open called on the GApplication instance; %d files", n_files);

    if (self->priv->desktop_manager == NULL) {
        init_desktop (self);
    }

    /* FIXME: how to do this? */
}

static void
kraken_desktop_application_open_location (KrakenApplication     *application,
                                        GFile               *location,
                                        GFile               *selection,
                                        const char          *startup_id,
                                        const gboolean      open_in_tabs)
{
    GAppInfo *appinfo;
    GError *error = NULL;
    GList *sel_list = NULL;

    appinfo = g_app_info_get_default_for_type ("inode/directory", TRUE);

    if (!appinfo) {
        g_warning ("Cannot launch file browser, no mimetype handler for inode/directory");
        return;
    }

    if (selection != NULL) {
        sel_list = g_list_prepend (sel_list, selection);
    } else if (location != NULL) {
        sel_list = g_list_prepend (sel_list, location);
    }

    if (!g_app_info_launch (appinfo, sel_list, NULL, &error)) {
        gchar *uri;

        uri = g_file_get_uri (selection);
        g_warning ("Could not launch file browser to display file: %s\n", uri);

        g_free (uri);
        g_clear_error (&error);
    }

    if (sel_list != NULL) {
        g_list_free (sel_list);
    }

    g_clear_object (&appinfo);
}

static KrakenWindow *
kraken_desktop_application_create_window (KrakenApplication *application,
                                        GdkScreen       *screen)
{
    return NULL;
}

static void
kraken_desktop_application_class_init (KrakenDesktopApplicationClass *class)
{
    GObjectClass *object_class;
    GApplicationClass *application_class;
    KrakenApplicationClass *kraken_app_class;

    object_class = G_OBJECT_CLASS (class);
    object_class->finalize = kraken_desktop_application_finalize;

    application_class = G_APPLICATION_CLASS (class);
    application_class->open = kraken_desktop_application_open;
    application_class->local_command_line = kraken_desktop_application_local_command_line;

    kraken_app_class = KRAKEN_APPLICATION_CLASS (class);
    kraken_app_class->continue_startup = kraken_desktop_application_continue_startup;
    kraken_app_class->create_window = kraken_desktop_application_create_window;
    kraken_app_class->continue_quit = kraken_desktop_application_continue_quit;
    kraken_app_class->open_location = kraken_desktop_application_open_location;

    g_type_class_add_private (class, sizeof (KrakenDesktopApplicationPriv));
}

KrakenApplication *
kraken_desktop_application_get_singleton (void)
{
    return kraken_application_initialize_singleton (KRAKEN_TYPE_DESKTOP_APPLICATION,
                                                  "application-id", "org.KrakenDesktop",
                                                  "flags", G_APPLICATION_HANDLES_OPEN,
                                                  "register-session", TRUE,
                                                  NULL);
}

