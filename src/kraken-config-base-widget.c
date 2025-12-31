/* kraken-config-base-widget.c */

/*  A base widget class for extension/action/script config widgets.
 *  This is usually part of a KrakenPluginManagerWidget
 */

#include <config.h>
#include "kraken-config-base-widget.h"
#include <glib.h>

G_DEFINE_TYPE (KrakenConfigBaseWidget, kraken_config_base_widget, GTK_TYPE_BIN);

static void
kraken_config_base_widget_class_init (KrakenConfigBaseWidgetClass *klass)
{
}

static void
kraken_config_base_widget_init (KrakenConfigBaseWidget *self)
{
    GtkStyleContext *context;
    GtkWidget *box;
    GtkWidget *w;
    GtkWidget *tb;
    GtkWidget *label;
    GtkWidget *toolbar_item;

    GtkWidget *frame = gtk_frame_new (NULL);
    gtk_container_add (GTK_CONTAINER (self), frame);
    gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_IN);

    context = gtk_widget_get_style_context (frame);
    gtk_style_context_add_class (context, "view");

    box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);

    gtk_container_add (GTK_CONTAINER (frame), box);

    tb = gtk_toolbar_new ();
    context = gtk_widget_get_style_context (tb);
    gtk_style_context_add_class (context, "primary-toolbar");
    gtk_box_pack_start (GTK_BOX (box), tb, FALSE, FALSE, 0);

    toolbar_item = GTK_WIDGET (gtk_tool_item_new ());
    gtk_container_add (GTK_CONTAINER (tb), toolbar_item);

    w = gtk_label_new (NULL);
    gtk_container_add (GTK_CONTAINER (toolbar_item), w);
    self->label = w;

    w = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (w), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start (GTK_BOX (box), w, TRUE, TRUE, 0);

    self->listbox = gtk_list_box_new ();
    gtk_list_box_set_selection_mode (GTK_LIST_BOX (self->listbox), GTK_SELECTION_NONE);
    gtk_container_add (GTK_CONTAINER (w), self->listbox);

    tb = gtk_toolbar_new ();
    context = gtk_widget_get_style_context (tb);
    gtk_style_context_add_class (context, GTK_STYLE_CLASS_TOOLBAR);
    gtk_box_pack_start (GTK_BOX (box), tb, FALSE, FALSE, 0);

    toolbar_item = GTK_WIDGET (gtk_tool_item_new ());
    gtk_tool_item_set_expand (GTK_TOOL_ITEM (toolbar_item), TRUE);
    gtk_container_add (GTK_CONTAINER (tb), toolbar_item);

    w = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_container_add (GTK_CONTAINER (toolbar_item), w);
    context = gtk_widget_get_style_context(w);
    gtk_style_context_add_class (context, "linked");
    self->lbuttonbox = w;

    label = gtk_label_new (_("Disable all"));
    gtk_label_set_ellipsize (GTK_LABEL (label), PANGO_ELLIPSIZE_END);

    self->disable_button = gtk_button_new ();
    gtk_container_add (GTK_CONTAINER (w), self->disable_button);
    gtk_container_add (GTK_CONTAINER (self->disable_button), label);
    gtk_widget_show_all (self->disable_button);
    gtk_widget_set_no_show_all (self->disable_button, TRUE);

    label = gtk_label_new (_("Enable all"));
    gtk_label_set_ellipsize (GTK_LABEL (label), PANGO_ELLIPSIZE_END);

    self->enable_button = gtk_button_new ();
    gtk_container_add (GTK_CONTAINER (w), self->enable_button);
    gtk_container_add (GTK_CONTAINER (self->enable_button), label);
    gtk_widget_show_all (self->enable_button);
    gtk_widget_set_no_show_all (self->enable_button, TRUE);

    toolbar_item = GTK_WIDGET (gtk_tool_item_new ());
    gtk_tool_item_set_expand (GTK_TOOL_ITEM (toolbar_item), FALSE);
    gtk_container_add (GTK_CONTAINER (tb), toolbar_item);

    w = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_container_add (GTK_CONTAINER (toolbar_item), w);
    context = gtk_widget_get_style_context(w);
    gtk_style_context_add_class (context, "linked");
    self->rbuttonbox = w;

    gtk_widget_show_all (GTK_WIDGET (self));
}

/**
 * kraken_config_base_widget_get_label:
 * @widget: a #KrakenConfigBaseWidget
 *
 * Returns: (transfer none): the label #GtkWidget
 */

GtkWidget *
kraken_config_base_widget_get_label (KrakenConfigBaseWidget *widget)
{
    return widget->label;
}

/**
 * kraken_config_base_widget_get_listbox:
 * @widget: a #KrakenConfigBaseWidget
 *
 * Returns: (transfer none): the listbox #GtkWidget
 */

GtkWidget *
kraken_config_base_widget_get_listbox (KrakenConfigBaseWidget *widget)
{
    return widget->listbox;
}

/**
 * kraken_config_base_widget_get_enable_button:
 * @widget: a #KrakenConfigBaseWidget
 *
 * Returns: (transfer none): the enable button #GtkWidget
 */

GtkWidget *
kraken_config_base_widget_get_enable_button (KrakenConfigBaseWidget *widget)
{
    return widget->enable_button;
}

/**
 * kraken_config_base_widget_get_disable_button:
 * @widget: a #KrakenConfigBaseWidget
 *
 * Returns: (transfer none): the disable button #GtkWidget
 */

GtkWidget *
kraken_config_base_widget_get_disable_button (KrakenConfigBaseWidget *widget)
{
    return widget->disable_button;
}

/**
 * kraken_config_base_widget_set_default_buttons_sensitive:
 * @widget: a #KrakenConfigBaseWidget
 * @sensitive: TRUE or FALSE
 *
 * Set the enable/disable buttons sensitive or not
 */

void
kraken_config_base_widget_set_default_buttons_sensitive (KrakenConfigBaseWidget *widget, gboolean sensitive)
{
    gtk_widget_set_sensitive (widget->enable_button, sensitive);
    gtk_widget_set_sensitive (widget->disable_button, sensitive);
}

/**
 * kraken_config_base_widget_clear_list:
 * @widget: a #KrakenConfigBaseWidget
 * 
 * Clear the listbox and destroy all children
 */

void
kraken_config_base_widget_clear_list (KrakenConfigBaseWidget *widget)
{
    gtk_container_foreach (GTK_CONTAINER (widget->listbox), (GtkCallback) gtk_widget_destroy, NULL);
}


