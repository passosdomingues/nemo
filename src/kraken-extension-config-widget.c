/* kraken-extension-config-widget.h */

/*  A widget that displays a list of extensions to enable or disable.
 *  This is usually part of a KrakenPluginManagerWidget
 */

#include <config.h>
#include "kraken-extension-config-widget.h"
#include "kraken-application.h"
#include "kraken-global-preferences.h"

#include <glib.h>

G_DEFINE_TYPE (KrakenExtensionConfigWidget, kraken_extension_config_widget, KRAKEN_TYPE_CONFIG_BASE_WIDGET);


typedef struct {
    KrakenExtensionConfigWidget *widget;

    gchar *name;
    gchar *display_name;
    gchar *desc;
    gchar *config_exec;
} ExtensionProxy;

static void
extension_proxy_free (ExtensionProxy *proxy)
{
    g_clear_pointer (&proxy->name, g_free);
    g_clear_pointer (&proxy->display_name, g_free);
    g_clear_pointer (&proxy->desc, g_free);
    g_clear_pointer (&proxy->config_exec, g_free);
    g_free (proxy);
}

static void
update_restart_visiblity (KrakenExtensionConfigWidget *widget)
{
    GList *tmp = g_list_copy (widget->initial_extension_ids);
    gchar **new_settings = g_settings_get_strv (kraken_plugin_preferences,
    		                                    KRAKEN_PLUGIN_PREFERENCES_DISABLED_EXTENSIONS);

    gboolean needs_restart = FALSE;

    if (g_list_length (tmp) != g_strv_length (new_settings)) {
        needs_restart = TRUE;
        goto out;
    }

    guint i;
    for (i = 0; i < g_strv_length (new_settings); i++) {
        GList *l = g_list_find_custom (tmp, new_settings[i], (GCompareFunc) g_strcmp0);
        if (!l) {
            needs_restart = TRUE;
            break;
        }
    }

out:
    g_strfreev (new_settings);
    g_list_free (tmp);

    gtk_widget_set_visible (widget->restart_button, needs_restart);
}

static GtkWidget *
get_button_for_row (GtkWidget *row)
{
    GtkWidget *ret;

    GtkWidget *box = gtk_bin_get_child (GTK_BIN (row));
    GList *clist = gtk_container_get_children (GTK_CONTAINER (box));

    ret = clist->data;

    g_list_free (clist);

    return ret;
}

static void
on_row_activated (GtkWidget *box, GtkWidget *row, GtkWidget *widget)
{
    GtkWidget *button = get_button_for_row (row);

    gtk_button_clicked (GTK_BUTTON (button));
}

static void
on_check_toggled(GtkWidget *button, ExtensionProxy *proxy)
{
    gboolean enabled = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button));

    gchar **blacklist = g_settings_get_strv (kraken_plugin_preferences,
    		                                 KRAKEN_PLUGIN_PREFERENCES_DISABLED_EXTENSIONS);

    GPtrArray *new_list = g_ptr_array_new ();

    guint i;

    if (enabled) {
        for (i = 0; i < g_strv_length (blacklist); i++) {
            if (g_strcmp0 (blacklist[i], proxy->name) == 0)
                continue;
            g_ptr_array_add (new_list, g_strdup (blacklist[i]));
        }
    } else {
        for (i = 0; i < g_strv_length (blacklist); i++) {
            g_ptr_array_add (new_list, g_strdup (blacklist[i]));
        }

        g_ptr_array_add (new_list, g_strdup (proxy->name));
    }

    g_ptr_array_add (new_list, NULL);

    gchar **new_list_ptr = (char **) g_ptr_array_free (new_list, FALSE);

    g_signal_handler_block (kraken_plugin_preferences, proxy->widget->bl_handler);
    g_settings_set_strv (kraken_plugin_preferences,
    		             KRAKEN_PLUGIN_PREFERENCES_DISABLED_EXTENSIONS,
						 (const gchar * const *)new_list_ptr);
    g_signal_handler_unblock (kraken_plugin_preferences, proxy->widget->bl_handler);

    update_restart_visiblity (proxy->widget);

    g_strfreev (blacklist);
    g_strfreev (new_list_ptr);
}

static gboolean
on_config_clicked (GtkLinkButton *button,
                   gpointer       user_data)
{
    ExtensionProxy *proxy = (ExtensionProxy *) user_data;

    g_spawn_command_line_async (proxy->config_exec, NULL);

    return GDK_EVENT_STOP;
}

#define LINE_PREFIX "KRAKEN_EXTENSION:::"
#define LINE_PREFIX_LEN 17

static void
detect_extensions (KrakenExtensionConfigWidget *widget)
{
    gchar *out = NULL;

    if (g_spawn_command_line_sync (LIBEXECDIR "/kraken-extensions-list",
                                   &out,
                                   NULL,
                                   NULL,
                                   NULL)) {
        if (out) {
            gchar **lines = g_strsplit (out, "\n", -1);

            g_free (out);

            guint i;
            for (i = 0; i < g_strv_length (lines); i++) {
                if (g_str_has_prefix (lines[i], LINE_PREFIX)) {
                    ExtensionProxy *p = g_new0 (ExtensionProxy, 1);

                    gchar **split = g_strsplit (lines[i] + LINE_PREFIX_LEN, ":::", -1);
                    gint len = g_strv_length (split);

                    if (len > 0) {
                        p->name = g_strdup (split[0]);
                    }

                    if (len == 3) {
                        p->display_name = g_strdup (split[1]);
                        p->desc = g_strdup (split[2]);
                    } else if (len == 4) {
                        p->display_name = g_strdup (split[1]);
                        p->desc = g_strdup (split[2]);
                        p->config_exec = g_strdup (split[3]);
                    } else {
                        p->display_name = NULL;
                        p->desc = NULL;
                        p->config_exec = NULL;
                    }

                    p->widget = widget;
                    widget->current_extensions = g_list_append (widget->current_extensions, p);
                    g_strfreev (split);
                }
            }
            g_strfreev (lines);
        }
    } else {
        g_printerr ("oops could not run kraken-extensions-list\n");
    }
}

static void
refresh_widget (KrakenExtensionConfigWidget *widget)
{
    if (widget->current_extensions != NULL) {
        g_list_free_full (widget->current_extensions, (GDestroyNotify) extension_proxy_free);
        widget->current_extensions = NULL;
    }

    kraken_config_base_widget_clear_list (KRAKEN_CONFIG_BASE_WIDGET (widget));

    detect_extensions (widget);

    if (widget->current_extensions == NULL) {
        GtkWidget *empty_label = gtk_label_new (NULL);
        gchar *markup = NULL;

        markup = g_strdup_printf ("<i>%s</i>", _("No extensions found"));

        gtk_label_set_markup (GTK_LABEL (empty_label), markup);
        g_free (markup);

        GtkWidget *empty_row = gtk_list_box_row_new ();
        gtk_container_add (GTK_CONTAINER (empty_row), empty_label);

        gtk_widget_show_all (empty_row);
        gtk_container_add (GTK_CONTAINER (KRAKEN_CONFIG_BASE_WIDGET (widget)->listbox), empty_row);
        gtk_widget_set_sensitive (GTK_WIDGET (KRAKEN_CONFIG_BASE_WIDGET (widget)->listbox), FALSE);
    } else {
        GtkSizeGroup *row_group, *name_group;
        GList *l;
        gchar **blacklist = g_settings_get_strv (kraken_plugin_preferences,
        		                                 KRAKEN_PLUGIN_PREFERENCES_DISABLED_EXTENSIONS);

        row_group = gtk_size_group_new (GTK_SIZE_GROUP_VERTICAL);
        name_group = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);

        for (l = widget->current_extensions; l != NULL; l=l->next) {
            gchar *markup;
            ExtensionProxy *proxy = l->data;

            gboolean active = TRUE;
            guint i = 0;

            for (i = 0; i < g_strv_length (blacklist); i++) {
                if (g_strcmp0 (blacklist[i], proxy->name) == 0) {
                    active = FALSE;
                    break;
                }
            }

            GtkWidget *w;
            GtkWidget *box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 2);

            GtkWidget *button = gtk_check_button_new ();

            gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), active);
            g_signal_connect (button, "toggled", G_CALLBACK (on_check_toggled), proxy);
            gtk_box_pack_start (GTK_BOX (box), button, FALSE, FALSE, 2);

            w = gtk_label_new (NULL);
            markup = NULL;

            if (proxy->display_name == NULL)
                markup = g_strdup_printf ("<b>%s</b>", proxy->name);
            else
                markup = g_strdup_printf ("<b>%s</b>", proxy->display_name);

            gtk_label_set_markup (GTK_LABEL (w), markup);
            gtk_label_set_xalign (GTK_LABEL (w), 0.0);

            g_clear_pointer (&markup, g_free);

            gtk_box_pack_start (GTK_BOX (box), w, FALSE, FALSE, 0);
            gtk_size_group_add_widget (name_group, w);

            w = gtk_label_new (NULL);
            gtk_label_set_lines (GTK_LABEL (w), 2);
            gtk_label_set_line_wrap (GTK_LABEL (w), TRUE);
            gtk_label_set_xalign (GTK_LABEL (w), 0.0);

            if (proxy->display_name == NULL)
                markup = g_strdup (_("no information available"));
            else
                markup = g_strdup_printf ("%s", proxy->desc);

            gtk_label_set_markup (GTK_LABEL (w), markup);
            g_clear_pointer (&markup, g_free);

            gtk_box_pack_start (GTK_BOX (box), w, FALSE, FALSE, 6);

            if (proxy->config_exec != NULL) {
                button = gtk_link_button_new_with_label ("", _("Configure"));
                g_signal_connect (button, "activate-link", G_CALLBACK (on_config_clicked), proxy);

                gtk_box_pack_end (GTK_BOX (box), button, FALSE, FALSE, 2);

                gtk_widget_set_tooltip_text (button, "");
            }

            GtkWidget *row = gtk_list_box_row_new ();
            gtk_container_add (GTK_CONTAINER (row), box);

            gtk_size_group_add_widget (row_group, row);

            gtk_widget_show_all (row);
            gtk_container_add (GTK_CONTAINER (KRAKEN_CONFIG_BASE_WIDGET (widget)->listbox), row);
        }

        g_strfreev (blacklist);
        g_object_unref (row_group);
        g_object_unref (name_group);
    }

    update_restart_visiblity (widget);
    kraken_config_base_widget_set_default_buttons_sensitive (KRAKEN_CONFIG_BASE_WIDGET (widget), widget->current_extensions != NULL);
}

static void
on_settings_changed (GSettings *settings, gchar *key, gpointer user_data)
{
    KrakenExtensionConfigWidget *w = KRAKEN_EXTENSION_CONFIG_WIDGET (user_data);

    update_restart_visiblity (w);
    refresh_widget (w);
}

static void
on_enable_clicked (GtkWidget *button, KrakenExtensionConfigWidget *widget)
{
    g_settings_set_strv (kraken_plugin_preferences,
    		             KRAKEN_PLUGIN_PREFERENCES_DISABLED_EXTENSIONS,
						 NULL);
}

static void
on_disable_clicked (GtkWidget *button, KrakenExtensionConfigWidget *widget)
{
    GPtrArray *new_list = g_ptr_array_new ();

    GList *l;

    for (l = widget->current_extensions; l != NULL; l = l->next)
        g_ptr_array_add (new_list, g_strdup (((ExtensionProxy *) l->data)->name));

    g_ptr_array_add (new_list, NULL);

    gchar **new_list_ptr = (char **) g_ptr_array_free (new_list, FALSE);
    g_settings_set_strv (kraken_plugin_preferences,
    		             KRAKEN_PLUGIN_PREFERENCES_DISABLED_EXTENSIONS,
						 (const gchar * const *) new_list_ptr);

    g_strfreev (new_list_ptr);
}

static void
on_restart_clicked (GtkWidget *button, KrakenExtensionConfigWidget *widget)
{
    /* TODO: We should be able to get existing window locations and geometry out of
     * gtk_application_get_windows() and restore them with the proper window geometry
     * and locations opened when restarting... dual pane, multiple tabs will probably
     * not be possible or exceedingly tedious, but we can cover the default view in
     * each window. */

    g_spawn_command_line_async ("sh -c \"kraken --quit && sleep 1 && kraken\"", NULL);
}

static void
kraken_extension_config_widget_finalize (GObject *object)
{
    KrakenExtensionConfigWidget *widget = KRAKEN_EXTENSION_CONFIG_WIDGET (object);

    if (widget->current_extensions != NULL) {
        g_list_free_full (widget->current_extensions, (GDestroyNotify) extension_proxy_free);
        widget->current_extensions = NULL;
    }

    g_list_free_full (widget->initial_extension_ids, (GDestroyNotify) g_free);

    g_signal_handler_disconnect (kraken_plugin_preferences, widget->bl_handler);

    G_OBJECT_CLASS (kraken_extension_config_widget_parent_class)->finalize (object);
}

static void
kraken_extension_config_widget_class_init (KrakenExtensionConfigWidgetClass *klass)
{
    GObjectClass *oclass;
    oclass = G_OBJECT_CLASS (klass);

    oclass->finalize = kraken_extension_config_widget_finalize;
}

static void
kraken_extension_config_widget_init (KrakenExtensionConfigWidget *self)
{
    self->current_extensions = NULL;
    self->initial_extension_ids = NULL;

    self->bl_handler = g_signal_connect (kraken_plugin_preferences,
    		                             "changed::" KRAKEN_PLUGIN_PREFERENCES_DISABLED_EXTENSIONS,
                                         G_CALLBACK (on_settings_changed), self);

    GtkWidget *label = kraken_config_base_widget_get_label (KRAKEN_CONFIG_BASE_WIDGET (self));

    gchar *title = g_strdup (_("Extensions"));
    gchar *markup = g_strdup_printf ("<b>%s</b>", title);

    gtk_label_set_markup (GTK_LABEL (label), markup);

    g_free (title);
    g_free (markup);

    self->restart_button = gtk_button_new_with_label (_("Extensions changed.  Restart required."));

    GtkWidget *bb = KRAKEN_CONFIG_BASE_WIDGET (self)->rbuttonbox;
    gtk_box_pack_end (GTK_BOX (bb),
                      self->restart_button,
                      FALSE, FALSE, 0);

    g_signal_connect (self->restart_button, "clicked", G_CALLBACK (on_restart_clicked), self);
    gtk_widget_set_no_show_all (self->restart_button, TRUE);

    g_signal_connect (kraken_config_base_widget_get_enable_button (KRAKEN_CONFIG_BASE_WIDGET (self)), "clicked",
                                                                 G_CALLBACK (on_enable_clicked), self);

    g_signal_connect (kraken_config_base_widget_get_disable_button (KRAKEN_CONFIG_BASE_WIDGET (self)), "clicked",
                                                                  G_CALLBACK (on_disable_clicked), self);

    g_signal_connect (KRAKEN_CONFIG_BASE_WIDGET (self)->listbox, "row-activated", G_CALLBACK (on_row_activated), self);

    gchar **init_settings = g_settings_get_strv (kraken_plugin_preferences,
    		                                     KRAKEN_PLUGIN_PREFERENCES_DISABLED_EXTENSIONS);
    guint i;
    for (i = 0; i < g_strv_length (init_settings); i++) {
        self->initial_extension_ids = g_list_append (self->initial_extension_ids, g_strdup (init_settings[i]));
    }

    g_strfreev (init_settings);

    refresh_widget (self);
}

GtkWidget *
kraken_extension_config_widget_new (void)
{
  return g_object_new (KRAKEN_TYPE_EXTENSION_CONFIG_WIDGET, NULL);
}
