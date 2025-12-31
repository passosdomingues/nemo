/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* fm-icon-view.c - implementation of icon view of directory.

   Copyright (C) 2000, 2001 Eazel, Inc.

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

#include <config.h>

#include "kraken-icon-view.h"

#include "kraken-actions.h"
#include "kraken-icon-view-container.h"
#include "kraken-icon-view-grid-container.h"
#include "kraken-error-reporting.h"
#include "kraken-view-dnd.h"
#include "kraken-view-factory.h"
#include "kraken-window.h"
#include "kraken-desktop-window.h"
#include "kraken-desktop-manager.h"
#include "kraken-application.h"

#include <stdlib.h>
#include <eel/eel-vfs-extensions.h>
#include <errno.h>
#include <fcntl.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <gio/gio.h>
#include <libkraken-private/kraken-clipboard-monitor.h>
#include <libkraken-private/kraken-directory.h>
#include <libkraken-private/kraken-dnd.h>
#include <libkraken-private/kraken-file-dnd.h>
#include <libkraken-private/kraken-file-utilities.h>
#include <libkraken-private/kraken-ui-utilities.h>
#include <libkraken-private/kraken-global-preferences.h>
#include <libkraken-private/kraken-icon-container.h>
#include <libkraken-private/kraken-icon-dnd.h>
#include <libkraken-private/kraken-icon.h>
#include <libkraken-private/kraken-link.h>
#include <libkraken-private/kraken-metadata.h>
#include <libkraken-private/kraken-clipboard.h>
#include <libkraken-private/kraken-desktop-icon-file.h>
#include <libkraken-private/kraken-desktop-utils.h>
#include <libkraken-private/kraken-desktop-directory.h>

#define DEBUG_FLAG KRAKEN_DEBUG_ICON_VIEW
#include <libkraken-private/kraken-debug.h>

#include <locale.h>
#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define POPUP_PATH_ICON_APPEARANCE		"/selection/Icon Appearance Items"

enum
{
	PROP_COMPACT = 1,
	PROP_SUPPORTS_AUTO_LAYOUT,
	PROP_IS_DESKTOP,
	PROP_SUPPORTS_KEEP_ALIGNED,
	PROP_SUPPORTS_LABELS_BESIDE_ICONS,
	NUM_PROPERTIES
};

static GParamSpec *properties[NUM_PROPERTIES] = { NULL, };

typedef struct {
	const KrakenFileSortType sort_type;
	const char *metadata_text;
	const char *action;
	const char *menu_label;
	const char *menu_hint;
} SortCriterion;

typedef enum {
	MENU_ITEM_TYPE_STANDARD,
	MENU_ITEM_TYPE_CHECK,
	MENU_ITEM_TYPE_RADIO,
	MENU_ITEM_TYPE_TREE
} MenuItemType;

struct KrakenIconViewDetails
{
	GList *icons_not_positioned;

	guint react_to_icon_change_idle_id;

	const SortCriterion *sort;
	gboolean sort_reversed;

	GtkActionGroup *icon_action_group;
	guint icon_merge_id;

	gboolean compact;

	gulong clipboard_handler_id;

	GtkWidget *icon_container;

	gboolean supports_auto_layout;
	gboolean is_desktop;
	gboolean supports_keep_aligned;
	gboolean supports_labels_beside_icons;
};


/* Note that the first item in this list is the default sort,
 * and that the items show up in the menu in the order they
 * appear in this list.
 */
static const SortCriterion sort_criteria[] = {
	{
		KRAKEN_FILE_SORT_BY_DISPLAY_NAME,
		"name",
		"Sort by Name",
		N_("by _Name"),
		N_("Keep icons sorted by name in rows")
	},
	{
		KRAKEN_FILE_SORT_BY_SIZE,
		"size",
		"Sort by Size",
		N_("by _Size"),
		N_("Keep icons sorted by size in rows")
	},
	{
		KRAKEN_FILE_SORT_BY_TYPE,
		"type",
		"Sort by Type",
		N_("by _Type"),
		N_("Keep icons sorted by type in rows")
	},
    {
        KRAKEN_FILE_SORT_BY_DETAILED_TYPE,
        "detailed_type",
        "Sort by Detailed Type",
        N_("by _Detailed Type"),
        N_("Keep icons sorted by detailed type in rows")
    },
	{
		KRAKEN_FILE_SORT_BY_MTIME,
		"modification date",
		"Sort by Modification Date",
		N_("by Modification _Date"),
		N_("Keep icons sorted by modification date in rows")
	},
	{
		KRAKEN_FILE_SORT_BY_TRASHED_TIME,
		"trashed",
		"Sort by Trash Time",
		N_("by T_rash Time"),
		N_("Keep icons sorted by trash time in rows")
	}
};

static void                 kraken_icon_view_set_directory_sort_by        (KrakenIconView           *icon_view,
									     KrakenFile         *file,
									     const char           *sort_by);
static void                 kraken_icon_view_set_zoom_level               (KrakenIconView           *view,
									     KrakenZoomLevel     new_level,
									     gboolean              always_emit);
static void                 kraken_icon_view_update_click_mode            (KrakenIconView           *icon_view);
static void                 kraken_icon_view_update_click_to_rename_mode  (KrakenIconView           *icon_view);
static gboolean             kraken_icon_view_is_desktop      (KrakenIconView           *icon_view);
static void                 kraken_icon_view_reveal_selection       (KrakenView               *view);
static const SortCriterion *get_sort_criterion_by_sort_type           (KrakenFileSortType  sort_type);
static void                 switch_to_manual_layout                   (KrakenIconView     *view);
static void                 update_layout_menus                       (KrakenIconView     *view);
static KrakenFileSortType get_default_sort_order                    (KrakenFile         *file,
								       gboolean             *reversed);
static void                 kraken_icon_view_clear_full                 (KrakenView *view,
                                                                       gboolean  destroying);
static const SortCriterion *get_sort_criterion_by_metadata_text (const char *metadata_text);
static void		    kraken_icon_view_remove_file (KrakenView *view, KrakenFile *file, KrakenDirectory *directory);

G_DEFINE_TYPE (KrakenIconView, kraken_icon_view, KRAKEN_TYPE_VIEW);

static void
kraken_icon_view_destroy (GtkWidget *object)
{
	KrakenIconView *icon_view;

	icon_view = KRAKEN_ICON_VIEW (object);

	kraken_icon_view_clear_full (KRAKEN_VIEW (object), TRUE);

        if (icon_view->details->react_to_icon_change_idle_id != 0) {
                g_source_remove (icon_view->details->react_to_icon_change_idle_id);
		icon_view->details->react_to_icon_change_idle_id = 0;
        }

	if (icon_view->details->clipboard_handler_id != 0) {
		g_signal_handler_disconnect (kraken_clipboard_monitor_get (),
					     icon_view->details->clipboard_handler_id);
		icon_view->details->clipboard_handler_id = 0;
	}

	if (icon_view->details->icons_not_positioned) {
		kraken_file_list_free (icon_view->details->icons_not_positioned);
		icon_view->details->icons_not_positioned = NULL;
	}

	GTK_WIDGET_CLASS (kraken_icon_view_parent_class)->destroy (object);
}

static void
sync_directory_monitor_number (KrakenIconView *view, KrakenFile *file)
{
    KrakenDirectory *directory;
    KrakenDesktopWindow *desktop_window;
    gint monitor;

    if (!view->details->is_desktop) {
        return;
    }

    desktop_window = KRAKEN_DESKTOP_WINDOW (kraken_view_get_kraken_window (KRAKEN_VIEW (view)));

    monitor = kraken_desktop_window_get_monitor (desktop_window);

    directory = kraken_view_get_model (KRAKEN_VIEW (view));

    KRAKEN_DESKTOP_DIRECTORY (directory)->display_number = monitor;
}


static KrakenIconContainer *
get_icon_container (KrakenIconView *icon_view)
{
	return KRAKEN_ICON_CONTAINER (icon_view->details->icon_container);
}

KrakenIconContainer *
kraken_icon_view_get_icon_container (KrakenIconView *icon_view)
{
	return get_icon_container (icon_view);
}

static gboolean
kraken_icon_view_supports_manual_layout (KrakenIconView *view)
{
	g_return_val_if_fail (KRAKEN_IS_ICON_VIEW (view), FALSE);

	return !kraken_icon_view_is_compact (view);
}

static void
real_set_sort_criterion (KrakenIconView *icon_view,
                         const SortCriterion *sort,
                         gboolean clear,
			 gboolean set_metadata)
{
	KrakenFile *file;

	file = kraken_view_get_directory_as_file (KRAKEN_VIEW (icon_view));

    sync_directory_monitor_number (icon_view, file);

	if (clear) {
		kraken_file_set_metadata (file,
                                KRAKEN_METADATA_KEY_ICON_VIEW_SORT_BY,
                                NULL,
                                NULL);
		kraken_file_set_metadata (file,
                                KRAKEN_METADATA_KEY_ICON_VIEW_SORT_REVERSED,
                                NULL,
                                NULL);
        icon_view->details->sort =
            get_sort_criterion_by_sort_type (get_default_sort_order (file, &icon_view->details->sort_reversed));
    } else if (set_metadata) {
		/* Store the new sort setting. */
        kraken_icon_view_set_directory_sort_by (icon_view,
                                              file,
                                              sort->metadata_text);
    }

	/* Update the layout menus to match the new sort setting. */
	update_layout_menus (icon_view);
}

static void
set_sort_criterion (KrakenIconView *icon_view,
		    const SortCriterion *sort,
		    gboolean set_metadata)
{
	if (sort == NULL ||
	    icon_view->details->sort == sort) {
		return;
	}

	icon_view->details->sort = sort;

        real_set_sort_criterion (icon_view, sort, FALSE, set_metadata);
}

static void
clear_sort_criterion (KrakenIconView *icon_view)
{
	real_set_sort_criterion (icon_view, NULL, TRUE, TRUE);
}

static void
kraken_icon_view_clean_up (KrakenIconView *icon_view)
{
	KrakenIconContainer *icon_container;
	gboolean saved_sort_reversed;

	icon_container = get_icon_container (icon_view);

	/* Hardwire Clean Up to always be by name, in forward order */
	saved_sort_reversed = icon_view->details->sort_reversed;

	kraken_icon_view_set_sort_reversed (icon_view, FALSE, FALSE);
	set_sort_criterion (icon_view, &sort_criteria[0], FALSE);

	kraken_icon_container_sort (icon_container);
	kraken_icon_container_freeze_icon_positions (icon_container);

	kraken_icon_view_set_sort_reversed (icon_view, saved_sort_reversed, FALSE);
}

static void
action_clean_up_callback (GtkAction *action, gpointer callback_data)
{
	kraken_icon_view_clean_up (KRAKEN_ICON_VIEW (callback_data));
}

static gboolean
kraken_icon_view_using_auto_layout (KrakenIconView *icon_view)
{
	return kraken_icon_container_is_auto_layout
		(get_icon_container (icon_view));
}

static void
action_sort_radio_callback (GtkAction *action,
			    GtkRadioAction *current,
			    KrakenIconView *view)
{
	KrakenFileSortType sort_type;

	sort_type = gtk_radio_action_get_current_value (current);

	/* Note that id might be a toggle item.
	 * Ignore non-sort ids so that they don't cause sorting.
	 */
	if (sort_type == KRAKEN_FILE_SORT_NONE) {
		switch_to_manual_layout (view);
	} else {
		kraken_icon_view_set_sort_criterion_by_sort_type (view, sort_type);
	}
}

static void
list_covers (KrakenIconData *data, gpointer callback_data)
{
	GSList **file_list;

	file_list = callback_data;

	*file_list = g_slist_prepend (*file_list, data);
}

static void
unref_cover (KrakenIconData *data, gpointer callback_data)
{
	kraken_file_unref (KRAKEN_FILE (data));
}

static void
kraken_icon_view_clear_full (KrakenView *view, gboolean destroying)
{
	KrakenIconContainer *icon_container;
	GSList *file_list;

	g_return_if_fail (KRAKEN_IS_ICON_VIEW (view));

	icon_container = get_icon_container (KRAKEN_ICON_VIEW (view));
	if (!icon_container)
		return;

	/* Clear away the existing icons. */
	file_list = NULL;
	kraken_icon_container_for_each (icon_container, list_covers, &file_list);
	kraken_icon_container_clear (icon_container);

    if (!destroying) {
        kraken_icon_container_update_scroll_region (icon_container);
    }

	g_slist_foreach (file_list, (GFunc)unref_cover, NULL);
	g_slist_free (file_list);
}

static void
kraken_icon_view_clear (KrakenView *view)
{
    kraken_icon_view_clear_full (view, FALSE);
}

static gboolean
should_show_file_on_screen (KrakenView *view, KrakenFile *file)
{
	if (!kraken_view_should_show_file (view, file)) {
		return FALSE;
	}

	return TRUE;
}

static void
kraken_icon_view_remove_file (KrakenView *view, KrakenFile *file, KrakenDirectory *directory)
{
	KrakenIconView *icon_view;

	/* This used to assert that 'directory == kraken_view_get_model (view)', but that
	 * resulted in a lot of crash reports (bug #352592). I don't see how that trace happens.
	 * It seems that somehow we get a files_changed event sent to the view from a directory
	 * that isn't the model, but the code disables the monitor and signal callback handlers when
	 * changing directories. Maybe we can get some more information when this happens.
	 * Further discussion in bug #368178.
	 */
	if (directory != kraken_view_get_model (view)) {
		char *file_uri, *dir_uri, *model_uri;
		file_uri = kraken_file_get_uri (file);
		dir_uri = kraken_directory_get_uri (directory);
		model_uri = kraken_directory_get_uri (kraken_view_get_model (view));
		g_warning ("kraken_icon_view_remove_file() - directory not icon view model, shouldn't happen.\n"
			   "file: %p:%s, dir: %p:%s, model: %p:%s, view loading: %d\n"
			   "If you see this, please add this info to http://bugzilla.gnome.org/show_bug.cgi?id=368178",
			   file, file_uri, directory, dir_uri, kraken_view_get_model (view), model_uri, kraken_view_get_loading (view));
		g_free (file_uri);
		g_free (dir_uri);
		g_free (model_uri);
	}

	icon_view = KRAKEN_ICON_VIEW (view);

	if (kraken_icon_container_remove (get_icon_container (icon_view),
					    KRAKEN_ICON_CONTAINER_ICON_DATA (file))) {
		kraken_file_unref (file);
	}
}

static void
kraken_icon_view_add_file (KrakenView *view, KrakenFile *file, KrakenDirectory *directory)
{
	KrakenIconView *icon_view;
	KrakenIconContainer *icon_container;

	g_assert (directory == kraken_view_get_model (view));

	icon_view = KRAKEN_ICON_VIEW (view);

    if (icon_view->details->is_desktop &&
        !should_show_file_on_screen (view, file)) {
        return;
    }

    icon_container = get_icon_container (icon_view);

    if (kraken_file_has_thumbnail_access_problem (file)) {
        kraken_application_set_cache_flag (kraken_application_get_singleton ());
        kraken_window_slot_check_bad_cache_bar (kraken_view_get_kraken_window_slot (view));
    }

	/* Reset scroll region for the first icon added when loading a directory. */
	if (kraken_view_get_loading (view) && kraken_icon_container_is_empty (icon_container)) {
		kraken_icon_container_reset_scroll_region (icon_container);
	}

	if (kraken_icon_container_add (icon_container,
					 KRAKEN_ICON_CONTAINER_ICON_DATA (file))) {
		kraken_file_ref (file);
	}
}

static void
kraken_icon_view_file_changed (KrakenView *view, KrakenFile *file, KrakenDirectory *directory)
{
	KrakenIconView *icon_view;

	g_assert (directory == kraken_view_get_model (view));

	g_return_if_fail (view != NULL);
	icon_view = KRAKEN_ICON_VIEW (view);

	if (!icon_view->details->is_desktop) {
		kraken_icon_container_request_update
			(get_icon_container (icon_view),
			 KRAKEN_ICON_CONTAINER_ICON_DATA (file));
		return;
	}

	if (!should_show_file_on_screen (view, file)) {
		kraken_icon_view_remove_file (view, file, directory);
	} else {

		kraken_icon_container_request_update
			(get_icon_container (icon_view),
			 KRAKEN_ICON_CONTAINER_ICON_DATA (file));
	}
}

static gboolean
kraken_icon_view_supports_auto_layout (KrakenIconView *view)
{
	g_return_val_if_fail (KRAKEN_IS_ICON_VIEW (view), FALSE);

	return view->details->supports_auto_layout;
}

static gboolean
kraken_icon_view_is_desktop (KrakenIconView *view)
{
	g_return_val_if_fail (KRAKEN_IS_ICON_VIEW (view), FALSE);

	return view->details->is_desktop;
}

static gboolean
kraken_icon_view_supports_keep_aligned (KrakenIconView *view)
{
	g_return_val_if_fail (KRAKEN_IS_ICON_VIEW (view), FALSE);

	return view->details->supports_keep_aligned;
}

static gboolean
kraken_icon_view_supports_labels_beside_icons (KrakenIconView *view)
{
	g_return_val_if_fail (KRAKEN_IS_ICON_VIEW (view), FALSE);

	return view->details->supports_labels_beside_icons;
}

static void
update_layout_menus (KrakenIconView *view)
{
	gboolean is_auto_layout;
	GtkAction *action;
	const char *action_name;
	KrakenFile *file;

	if (view->details->icon_action_group == NULL) {
		return;
	}

	is_auto_layout = kraken_icon_view_using_auto_layout (view);
	file = kraken_view_get_directory_as_file (KRAKEN_VIEW (view));

	if (kraken_icon_view_supports_auto_layout (view)) {
		/* Mark sort criterion. */
		action_name = is_auto_layout ? view->details->sort->action : KRAKEN_ACTION_MANUAL_LAYOUT;
		action = gtk_action_group_get_action (view->details->icon_action_group,
						      action_name);
		gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), TRUE);

        action = gtk_action_group_get_action (view->details->icon_action_group,
                                              KRAKEN_ACTION_REVERSED_ORDER);
		gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action),
					      view->details->sort_reversed);
		gtk_action_set_sensitive (action, is_auto_layout);

		action = gtk_action_group_get_action (view->details->icon_action_group,
		                                      KRAKEN_ACTION_SORT_TRASH_TIME);

		if (file != NULL && kraken_file_is_in_trash (file)) {
			gtk_action_set_visible (action, TRUE);
		} else {
			gtk_action_set_visible (action, FALSE);
		}
	}

	action = gtk_action_group_get_action (view->details->icon_action_group,
					      KRAKEN_ACTION_MANUAL_LAYOUT);
	gtk_action_set_visible (action,
				kraken_icon_view_supports_manual_layout (view));

	/* Clean Up is only relevant for manual layout */
	action = gtk_action_group_get_action (view->details->icon_action_group,
					      KRAKEN_ACTION_CLEAN_UP);
	gtk_action_set_sensitive (action, !is_auto_layout);

	if (kraken_icon_view_is_desktop (view)) {
		gtk_action_set_label (action, _("_Organize Desktop by Name"));
	}

	action = gtk_action_group_get_action (view->details->icon_action_group,
					      KRAKEN_ACTION_KEEP_ALIGNED);
	gtk_action_set_visible (action,
				kraken_icon_view_supports_keep_aligned (view));
	gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action),
				      kraken_icon_container_is_keep_aligned (get_icon_container (view)));
	gtk_action_set_sensitive (action, !is_auto_layout);
}


gchar *
kraken_icon_view_get_directory_sort_by (KrakenIconView *icon_view,
					  KrakenFile *file)
{
	const SortCriterion *default_sort_criterion;

	if (!kraken_icon_view_supports_auto_layout (icon_view)) {
		return g_strdup ("name");
	}

	default_sort_criterion = get_sort_criterion_by_sort_type (get_default_sort_order (file, NULL));
	g_return_val_if_fail (default_sort_criterion != NULL, NULL);

    sync_directory_monitor_number (icon_view, file);

    return kraken_file_get_metadata (file,
                                   KRAKEN_METADATA_KEY_ICON_VIEW_SORT_BY,
                                   default_sort_criterion->metadata_text);
}

static KrakenFileSortType
get_default_sort_order (KrakenFile *file, gboolean *reversed)
{
	KrakenFileSortType retval, default_sort_order;
	gboolean default_sort_in_reverse_order;

	default_sort_order = g_settings_get_enum (kraken_preferences,
						  KRAKEN_PREFERENCES_DEFAULT_SORT_ORDER);
	default_sort_in_reverse_order = g_settings_get_boolean (kraken_preferences,
								KRAKEN_PREFERENCES_DEFAULT_SORT_IN_REVERSE_ORDER);

	retval = kraken_file_get_default_sort_type (file, reversed);

	if (retval == KRAKEN_FILE_SORT_NONE) {

		if (reversed != NULL) {
			*reversed = default_sort_in_reverse_order;
		}

		retval = CLAMP (default_sort_order, KRAKEN_FILE_SORT_BY_DISPLAY_NAME,
				KRAKEN_FILE_SORT_BY_MTIME);
	}

	return retval;
}

static void
kraken_icon_view_set_directory_sort_by (KrakenIconView *icon_view,
					  KrakenFile *file,
					  const char *sort_by)
{
	const SortCriterion *default_sort_criterion;

	if (!kraken_icon_view_supports_auto_layout (icon_view)) {
		return;
	}

	default_sort_criterion = get_sort_criterion_by_sort_type (get_default_sort_order (file, NULL));
	g_return_if_fail (default_sort_criterion != NULL);

    sync_directory_monitor_number (icon_view, file);

    kraken_file_set_metadata (file,
                            KRAKEN_METADATA_KEY_ICON_VIEW_SORT_BY,
                            default_sort_criterion->metadata_text,
                            sort_by);
}

gboolean
kraken_icon_view_get_directory_sort_reversed (KrakenIconView *icon_view,
						KrakenFile *file)
{
	gboolean reversed;

	if (!kraken_icon_view_supports_auto_layout (icon_view)) {
		return FALSE;
	}

	get_default_sort_order (file, &reversed);

    sync_directory_monitor_number (icon_view, file);

    return kraken_file_get_boolean_metadata (file,
                                           KRAKEN_METADATA_KEY_ICON_VIEW_SORT_REVERSED,
                                           reversed);
}

static void
kraken_icon_view_set_directory_sort_reversed (KrakenIconView *icon_view,
						KrakenFile *file,
						gboolean sort_reversed)
{
	gboolean reversed;

	if (!kraken_icon_view_supports_auto_layout (icon_view)) {
		return;
	}

	get_default_sort_order (file, &reversed);

    sync_directory_monitor_number (icon_view, file);

    kraken_file_set_boolean_metadata (file,
                                    KRAKEN_METADATA_KEY_ICON_VIEW_SORT_REVERSED,
                                    reversed,
                                    sort_reversed);
}

static gboolean
get_default_directory_keep_aligned (void)
{
	return TRUE;
}

static gboolean
kraken_icon_view_get_directory_keep_aligned (KrakenIconView *icon_view,
					       KrakenFile *file)
{
	if (!kraken_icon_view_supports_keep_aligned (icon_view)) {
		return FALSE;
	}

    sync_directory_monitor_number (icon_view, file);

    return kraken_file_get_boolean_metadata (file,
                                           KRAKEN_METADATA_KEY_ICON_VIEW_KEEP_ALIGNED,
                                           get_default_directory_keep_aligned ());
}

void
kraken_icon_view_set_directory_keep_aligned (KrakenIconView *icon_view,
					       KrakenFile *file,
					       gboolean keep_aligned)
{
	if (!kraken_icon_view_supports_keep_aligned (icon_view)) {
		return;
	}

    sync_directory_monitor_number (icon_view, file);

    kraken_file_set_boolean_metadata (file,
                                    KRAKEN_METADATA_KEY_ICON_VIEW_KEEP_ALIGNED,
                                    get_default_directory_keep_aligned (),
                                    keep_aligned);
}

static gboolean
kraken_icon_view_get_directory_auto_layout (KrakenIconView *icon_view,
					      KrakenFile *file)
{
	if (!kraken_icon_view_supports_auto_layout (icon_view)) {
		return FALSE;
	}

	if (!kraken_icon_view_supports_manual_layout (icon_view)) {
		return TRUE;
	}

    sync_directory_monitor_number (icon_view, file);

    return kraken_file_get_boolean_metadata (file,
                                           KRAKEN_METADATA_KEY_ICON_VIEW_AUTO_LAYOUT,
                                           TRUE);
}

static void
kraken_icon_view_set_directory_auto_layout (KrakenIconView *icon_view,
					      KrakenFile *file,
					gboolean auto_layout)
{
	if (!kraken_icon_view_supports_auto_layout (icon_view) ||
	    !kraken_icon_view_supports_manual_layout (icon_view)) {
		return;
	}

    sync_directory_monitor_number (icon_view, file);

    kraken_file_set_boolean_metadata (file,
                                    KRAKEN_METADATA_KEY_ICON_VIEW_AUTO_LAYOUT,
                                    TRUE,
                                    auto_layout);
}

void
kraken_icon_view_set_directory_horizontal_layout (KrakenIconView *icon_view,
                                                KrakenFile     *file,
                                                gboolean      horizontal)
{
    sync_directory_monitor_number (icon_view, file);

    kraken_file_set_boolean_metadata (file,
                                    KRAKEN_METADATA_KEY_DESKTOP_GRID_HORIZONTAL,
                                    FALSE,
                                    horizontal);
}

gboolean
kraken_icon_view_get_directory_horizontal_layout (KrakenIconView *icon_view,
                                                KrakenFile     *file)
{
    sync_directory_monitor_number (icon_view, file);

    return kraken_file_get_boolean_metadata (file,
                                           KRAKEN_METADATA_KEY_DESKTOP_GRID_HORIZONTAL,
                                           FALSE);
}

void
kraken_icon_view_set_directory_grid_adjusts (KrakenIconView *icon_view,
                                           KrakenFile     *file,
                                           gint          horizontal,
                                           gint          vertical)
{
    sync_directory_monitor_number (icon_view, file);

    kraken_file_set_desktop_grid_adjusts (file,
                                        KRAKEN_METADATA_KEY_DESKTOP_GRID_ADJUST,
                                        horizontal, vertical);
}

void
kraken_icon_view_get_directory_grid_adjusts (KrakenIconView *icon_view,
                                           KrakenFile     *file,
                                           gint         *horizontal,
                                           gint         *vertical)
{
    gint h, v;

    sync_directory_monitor_number (icon_view, file);

    kraken_file_get_desktop_grid_adjusts (file,
                                        KRAKEN_METADATA_KEY_DESKTOP_GRID_ADJUST,
                                        &h, &v);

    if (horizontal)
        *horizontal = h;

    if (vertical)
        *vertical = v;
}

gboolean
kraken_icon_view_set_sort_reversed (KrakenIconView *icon_view,
                                  gboolean      new_value,
                                  gboolean      set_metadata)
{
    if (icon_view->details->sort_reversed == new_value) {
        return FALSE;
    }

    icon_view->details->sort_reversed = new_value;

    if (set_metadata) {
        /* Store the new sort setting. */
        kraken_icon_view_set_directory_sort_reversed (icon_view, kraken_view_get_directory_as_file (KRAKEN_VIEW (icon_view)), new_value);
    }

    /* Update the layout menus to match the new sort-order setting. */
    update_layout_menus (icon_view);

    return TRUE;
}

void
kraken_icon_view_flip_sort_reversed (KrakenIconView *icon_view)
{
    kraken_icon_view_set_sort_reversed (icon_view, !icon_view->details->sort_reversed, TRUE);
}

static const SortCriterion *
get_sort_criterion_by_metadata_text (const char *metadata_text)
{
	guint i;

	/* Figure out what the new sort setting should be. */
	for (i = 0; i < G_N_ELEMENTS (sort_criteria); i++) {
		if (g_strcmp0 (sort_criteria[i].metadata_text, metadata_text) == 0) {
			return &sort_criteria[i];
		}
	}
	return NULL;
}

static const SortCriterion *
get_sort_criterion_by_sort_type (KrakenFileSortType sort_type)
{
	guint i;

	/* Figure out what the new sort setting should be. */
	for (i = 0; i < G_N_ELEMENTS (sort_criteria); i++) {
		if (sort_type == sort_criteria[i].sort_type) {
			return &sort_criteria[i];
		}
	}

	return &sort_criteria[0];
}

#define DEFAULT_ZOOM_LEVEL(icon_view) icon_view->details->compact ? default_compact_zoom_level : default_zoom_level

static KrakenZoomLevel
get_default_zoom_level (KrakenIconView *icon_view)
{
	KrakenZoomLevel default_zoom_level, default_compact_zoom_level;

	default_zoom_level = g_settings_get_enum (kraken_icon_view_preferences,
						  KRAKEN_PREFERENCES_ICON_VIEW_DEFAULT_ZOOM_LEVEL);
	default_compact_zoom_level = g_settings_get_enum (kraken_compact_view_preferences,
							  KRAKEN_PREFERENCES_COMPACT_VIEW_DEFAULT_ZOOM_LEVEL);

    if (KRAKEN_ICON_VIEW_GET_CLASS (icon_view)->use_grid_container) {
        return KRAKEN_ZOOM_LEVEL_STANDARD;
    }

	return CLAMP (DEFAULT_ZOOM_LEVEL(icon_view), KRAKEN_ZOOM_LEVEL_SMALLEST, KRAKEN_ZOOM_LEVEL_LARGEST);
}

static void
set_labels_beside_icons (KrakenIconView *icon_view)
{
	gboolean labels_beside;

	if (kraken_icon_view_supports_labels_beside_icons (icon_view)) {
		labels_beside = kraken_icon_view_is_compact (icon_view) ||
			g_settings_get_boolean (kraken_icon_view_preferences,
						KRAKEN_PREFERENCES_ICON_VIEW_LABELS_BESIDE_ICONS);

		if (labels_beside) {
			kraken_icon_container_set_label_position
				(get_icon_container (icon_view),
				 KRAKEN_ICON_LABEL_POSITION_BESIDE);
		} else {
			kraken_icon_container_set_label_position
				(get_icon_container (icon_view),
				 KRAKEN_ICON_LABEL_POSITION_UNDER);
		}
	}
}

static void
set_columns_same_width (KrakenIconView *icon_view)
{
	gboolean all_columns_same_width;

	if (kraken_icon_view_is_compact (icon_view)) {
		all_columns_same_width = g_settings_get_boolean (kraken_compact_view_preferences,
								 KRAKEN_PREFERENCES_COMPACT_VIEW_ALL_COLUMNS_SAME_WIDTH);
		kraken_icon_container_set_all_columns_same_width (get_icon_container (icon_view), all_columns_same_width);
	}
}

static void
kraken_icon_view_begin_loading (KrakenView *view)
{
	KrakenIconView *icon_view;
	GtkWidget *icon_container;
	KrakenFile *file;
	int level;
    int h_adjust, v_adjust;
	char *sort_name, *uri;

	g_return_if_fail (KRAKEN_IS_ICON_VIEW (view));

	icon_view = KRAKEN_ICON_VIEW (view);
	file = kraken_view_get_directory_as_file (view);
	uri = kraken_file_get_uri (file);
	icon_container = GTK_WIDGET (get_icon_container (icon_view));

    kraken_icon_container_set_ok_to_load_deferred_attrs (KRAKEN_ICON_CONTAINER (icon_container), FALSE);

	kraken_icon_container_begin_loading (KRAKEN_ICON_CONTAINER (icon_container));

	kraken_icon_container_set_allow_moves (KRAKEN_ICON_CONTAINER (icon_container),
						 !eel_uri_is_search (uri));

	g_free (uri);

	/* Set up the zoom level from the metadata. */
	if (kraken_view_supports_zooming (KRAKEN_VIEW (icon_view))) {
        if (kraken_global_preferences_get_ignore_view_metadata () && !KRAKEN_ICON_VIEW_GET_CLASS (view)->use_grid_container) {
            if (kraken_window_get_ignore_meta_zoom_level (kraken_view_get_kraken_window (view)) == -1) {
                kraken_window_set_ignore_meta_zoom_level (kraken_view_get_kraken_window (view), get_default_zoom_level (icon_view));
            }

            level = kraken_window_get_ignore_meta_zoom_level (kraken_view_get_kraken_window (KRAKEN_VIEW (icon_view)));
        } else {
            sync_directory_monitor_number (icon_view, file);

            if (icon_view->details->compact) {
                level = kraken_file_get_integer_metadata (file,
                                                        KRAKEN_METADATA_KEY_COMPACT_VIEW_ZOOM_LEVEL,
                                                        get_default_zoom_level (icon_view));
            } else {
                level = kraken_file_get_integer_metadata (file,
                                                        KRAKEN_METADATA_KEY_ICON_VIEW_ZOOM_LEVEL,
                                                        get_default_zoom_level (icon_view));
    		}
        }

		kraken_icon_view_set_zoom_level (icon_view, level, TRUE);
	}

	/* Set the sort mode.
	 * It's OK not to resort the icons because the
	 * container doesn't have any icons at this point.
	 */
	sort_name = kraken_icon_view_get_directory_sort_by (icon_view, file);
	set_sort_criterion (icon_view, get_sort_criterion_by_metadata_text (sort_name), FALSE);
	g_free (sort_name);

	/* Set the sort direction from the metadata. */
	kraken_icon_view_set_sort_reversed (icon_view, kraken_icon_view_get_directory_sort_reversed (icon_view, file), FALSE);

    kraken_icon_container_set_horizontal_layout (get_icon_container (icon_view),
                                               kraken_icon_view_get_directory_horizontal_layout (icon_view, file));

	kraken_icon_container_set_keep_aligned (get_icon_container (icon_view),
                    kraken_icon_view_get_directory_keep_aligned (icon_view, file));

    kraken_icon_view_get_directory_grid_adjusts (KRAKEN_ICON_VIEW (view),
                                               file,
                                               &h_adjust,
                                               &v_adjust);

    kraken_icon_container_set_grid_adjusts (get_icon_container (icon_view), h_adjust, v_adjust);

	set_labels_beside_icons (icon_view);
	set_columns_same_width (icon_view);

	/* We must set auto-layout last, because it invokes the layout_changed
	 * callback, which works incorrectly if the other layout criteria are
	 * not already set up properly (see bug 6500, e.g.)
	 */
	kraken_icon_container_set_auto_layout
		(get_icon_container (icon_view),
		 kraken_icon_view_get_directory_auto_layout (icon_view, file));

	/* e.g. keep aligned may have changed */
	update_layout_menus (icon_view);
}

static void
icon_view_notify_clipboard_info (KrakenClipboardMonitor *monitor,
                                 KrakenClipboardInfo *info,
                                 KrakenIconView *icon_view)
{
	GList *icon_data;

	icon_data = NULL;
	if (info && info->cut) {
		icon_data = info->files;
	}

	kraken_icon_container_set_highlighted_for_clipboard (
							       get_icon_container (icon_view), icon_data);
}

static void
kraken_icon_view_end_loading (KrakenView *view,
			  gboolean all_files_seen)
{
	KrakenIconView *icon_view;
	GtkWidget *icon_container;
	KrakenClipboardMonitor *monitor;
	KrakenClipboardInfo *info;

	icon_view = KRAKEN_ICON_VIEW (view);

	icon_container = GTK_WIDGET (get_icon_container (icon_view));
	kraken_icon_container_end_loading (KRAKEN_ICON_CONTAINER (icon_container), all_files_seen);

	monitor = kraken_clipboard_monitor_get ();
	info = kraken_clipboard_monitor_get_clipboard_info (monitor);
    kraken_icon_container_set_ok_to_load_deferred_attrs (KRAKEN_ICON_CONTAINER (icon_container), TRUE);
	icon_view_notify_clipboard_info (monitor, info, icon_view);
}

static KrakenZoomLevel
kraken_icon_view_get_zoom_level (KrakenView *view)
{
	g_return_val_if_fail (KRAKEN_IS_ICON_VIEW (view), KRAKEN_ZOOM_LEVEL_STANDARD);

	return kraken_icon_container_get_zoom_level (get_icon_container (KRAKEN_ICON_VIEW (view)));
}

static void
kraken_icon_view_set_zoom_level (KrakenIconView *view,
				   KrakenZoomLevel new_level,
				   gboolean always_emit)
{
	KrakenIconContainer *icon_container;

	g_return_if_fail (KRAKEN_IS_ICON_VIEW (view));
	g_return_if_fail (new_level >= KRAKEN_ZOOM_LEVEL_SMALLEST &&
			  new_level <= KRAKEN_ZOOM_LEVEL_LARGEST);

	icon_container = get_icon_container (view);
	if (kraken_icon_container_get_zoom_level (icon_container) == new_level) {
		if (always_emit) {
			g_signal_emit_by_name (view, "zoom_level_changed");
		}
		return;
	}

    if (kraken_global_preferences_get_ignore_view_metadata () && !KRAKEN_ICON_VIEW_GET_CLASS (view)->use_grid_container) {
        kraken_window_set_ignore_meta_zoom_level (kraken_view_get_kraken_window (KRAKEN_VIEW (view)), new_level);
    } else {
        sync_directory_monitor_number (view, kraken_view_get_directory_as_file (KRAKEN_VIEW (view)));

        if (view->details->compact) {
            kraken_file_set_integer_metadata (kraken_view_get_directory_as_file (KRAKEN_VIEW (view)),
                                            KRAKEN_METADATA_KEY_COMPACT_VIEW_ZOOM_LEVEL,
                                            get_default_zoom_level (view),
                                            new_level);
        } else {
            kraken_file_set_integer_metadata (kraken_view_get_directory_as_file (KRAKEN_VIEW (view)),
                                            KRAKEN_METADATA_KEY_ICON_VIEW_ZOOM_LEVEL,
                                            get_default_zoom_level (view),
                                            new_level);
        }
    }

	kraken_icon_container_set_zoom_level (icon_container, new_level);

	g_signal_emit_by_name (view, "zoom_level_changed");

	if (kraken_view_get_active (KRAKEN_VIEW (view))) {
		kraken_view_update_menus (KRAKEN_VIEW (view));
	}
}

static void
kraken_icon_view_bump_zoom_level (KrakenView *view, int zoom_increment)
{
	KrakenZoomLevel new_level;

	g_return_if_fail (KRAKEN_IS_ICON_VIEW (view));

	new_level = kraken_icon_view_get_zoom_level (view) + zoom_increment;

	if (new_level >= KRAKEN_ZOOM_LEVEL_SMALLEST &&
	    new_level <= KRAKEN_ZOOM_LEVEL_LARGEST) {
		kraken_view_zoom_to_level (view, new_level);
	}
}

static void
kraken_icon_view_zoom_to_level (KrakenView *view,
			    KrakenZoomLevel zoom_level)
{
	KrakenIconView *icon_view;

	g_assert (KRAKEN_IS_ICON_VIEW (view));

	icon_view = KRAKEN_ICON_VIEW (view);
	kraken_icon_view_set_zoom_level (icon_view, zoom_level, FALSE);
}

static void
kraken_icon_view_restore_default_zoom_level (KrakenView *view)
{
	KrakenIconView *icon_view;

	g_return_if_fail (KRAKEN_IS_ICON_VIEW (view));

	icon_view = KRAKEN_ICON_VIEW (view);
	kraken_view_zoom_to_level
		(view, get_default_zoom_level (icon_view));
}

static KrakenZoomLevel
kraken_icon_view_get_default_zoom_level (KrakenView *view)
{
    g_return_val_if_fail (KRAKEN_IS_ICON_VIEW (view), KRAKEN_ZOOM_LEVEL_NULL);

    return get_default_zoom_level(KRAKEN_ICON_VIEW (view));
}

static gboolean
kraken_icon_view_can_zoom_in (KrakenView *view)
{
	g_return_val_if_fail (KRAKEN_IS_ICON_VIEW (view), FALSE);

	return kraken_icon_view_get_zoom_level (view)
		< KRAKEN_ZOOM_LEVEL_LARGEST;
}

static gboolean
kraken_icon_view_can_zoom_out (KrakenView *view)
{
	g_return_val_if_fail (KRAKEN_IS_ICON_VIEW (view), FALSE);

	return kraken_icon_view_get_zoom_level (view)
		> KRAKEN_ZOOM_LEVEL_SMALLEST;
}

static gboolean
kraken_icon_view_is_empty (KrakenView *view)
{
	g_assert (KRAKEN_IS_ICON_VIEW (view));

	return kraken_icon_container_is_empty
		(get_icon_container (KRAKEN_ICON_VIEW (view)));
}

static GList *
kraken_icon_view_get_selection (KrakenView *view)
{
	GList *list;

	g_return_val_if_fail (KRAKEN_IS_ICON_VIEW (view), NULL);

	list = kraken_icon_container_get_selection
		(get_icon_container (KRAKEN_ICON_VIEW (view)));
	kraken_file_list_ref (list);
	return list;
}

static GList *
kraken_icon_view_peek_selection (KrakenView *view)
{
    GList *list;

    g_return_val_if_fail (KRAKEN_IS_ICON_VIEW (view), NULL);

    list = kraken_icon_container_peek_selection (get_icon_container (KRAKEN_ICON_VIEW (view)));
    kraken_file_list_ref (list);
    return list;
}

static gint
kraken_icon_view_get_selection_count (KrakenView *view)
{
    g_return_val_if_fail (KRAKEN_IS_ICON_VIEW (view), 0);

    return kraken_icon_container_get_selection_count (get_icon_container (KRAKEN_ICON_VIEW (view)));
}

static void
count_item (KrakenIconData *icon_data,
	    gpointer callback_data)
{
	guint *count;

	count = callback_data;
	(*count)++;
}

static guint
kraken_icon_view_get_item_count (KrakenView *view)
{
	guint count;

	g_return_val_if_fail (KRAKEN_IS_ICON_VIEW (view), 0);

	count = 0;

	kraken_icon_container_for_each
		(get_icon_container (KRAKEN_ICON_VIEW (view)),
		 count_item, &count);

	return count;
}

void
kraken_icon_view_set_sort_criterion_by_sort_type (KrakenIconView     *icon_view,
                                                KrakenFileSortType  sort_type)
{
	const SortCriterion *sort;

	g_assert (KRAKEN_IS_ICON_VIEW (icon_view));

	sort = get_sort_criterion_by_sort_type (sort_type);
	g_return_if_fail (sort != NULL);

	if (sort == icon_view->details->sort
	    && kraken_icon_view_using_auto_layout (icon_view)) {
		return;
	}

	set_sort_criterion (icon_view, sort, TRUE);
	kraken_icon_container_sort (get_icon_container (icon_view));
	kraken_icon_view_reveal_selection (KRAKEN_VIEW (icon_view));
}


static void
action_reversed_order_callback (GtkAction *action,
				gpointer user_data)
{
	KrakenIconView *icon_view;

	icon_view = KRAKEN_ICON_VIEW (user_data);

	if (kraken_icon_view_set_sort_reversed (icon_view,
			       gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action)),
			       TRUE)) {
		kraken_icon_container_sort (get_icon_container (icon_view));
		kraken_icon_view_reveal_selection (KRAKEN_VIEW (icon_view));
	}
}

static void
action_keep_aligned_callback (GtkAction *action,
			      gpointer user_data)
{
	KrakenIconView *icon_view;
	KrakenFile *file;
	gboolean keep_aligned;

	icon_view = KRAKEN_ICON_VIEW (user_data);

	keep_aligned = gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action));

	file = kraken_view_get_directory_as_file (KRAKEN_VIEW (icon_view));
	kraken_icon_view_set_directory_keep_aligned (icon_view,
						 file,
						 keep_aligned);

	kraken_icon_container_set_keep_aligned (get_icon_container (icon_view),
						  keep_aligned);
}

static void
switch_to_manual_layout (KrakenIconView *icon_view)
{
	if (!kraken_icon_view_using_auto_layout (icon_view)) {
		return;
	}

	icon_view->details->sort = &sort_criteria[0];

	kraken_icon_container_set_auto_layout
		(get_icon_container (icon_view), FALSE);
}

static void
layout_changed_callback (KrakenIconContainer *container,
			 KrakenIconView *icon_view)
{
	KrakenFile *file;

	g_assert (KRAKEN_IS_ICON_VIEW (icon_view));
	g_assert (container == get_icon_container (icon_view));

	file = kraken_view_get_directory_as_file (KRAKEN_VIEW (icon_view));

	if (file != NULL) {
		kraken_icon_view_set_directory_auto_layout
			(icon_view,
			 file,
			 kraken_icon_view_using_auto_layout (icon_view));
	}

	update_layout_menus (icon_view);
}

static gboolean
kraken_icon_view_can_rename_file (KrakenView *view, KrakenFile *file)
{
	if (!(kraken_icon_view_get_zoom_level (view) > KRAKEN_ZOOM_LEVEL_SMALLEST)) {
		return FALSE;
	}

	return KRAKEN_VIEW_CLASS(kraken_icon_view_parent_class)->can_rename_file (view, file);
}

static void
kraken_icon_view_start_renaming_file (KrakenView *view,
				  KrakenFile *file,
				  gboolean select_all)
{
	/* call parent class to make sure the right icon is selected */
	KRAKEN_VIEW_CLASS(kraken_icon_view_parent_class)->start_renaming_file (view, file, select_all);

	/* start renaming */
	kraken_icon_container_start_renaming_selected_item
		(get_icon_container (KRAKEN_ICON_VIEW (view)), select_all);
}

static const GtkActionEntry icon_view_entries[] = {
  /* name, stock id, label */  { "Arrange Items", NULL, N_("Arran_ge Items") },
  /* name, stock id */         { "Clean Up", NULL,
  /* label, accelerator */       N_("_Organize by Name"), NULL,
  /* tooltip */                  N_("Reposition icons to better fit in the window and avoid overlapping"),
                                 G_CALLBACK (action_clean_up_callback) },
};

static const GtkToggleActionEntry icon_view_toggle_entries[] = {

  /* name, stock id */      { "Reversed Order", NULL,
  /* label, accelerator */    N_("Re_versed Order"), NULL,
  /* tooltip */               N_("Display icons in the opposite order"),
                              G_CALLBACK (action_reversed_order_callback),
                              0 },
  /* name, stock id */      { "Keep Aligned", NULL,
  /* label, accelerator */    N_("_Keep Aligned"), NULL,
  /* tooltip */               N_("Keep icons lined up on a grid"),
                              G_CALLBACK (action_keep_aligned_callback),
                              0 },
};

static const GtkRadioActionEntry arrange_radio_entries[] = {
  { "Manual Layout", NULL,
    N_("_Manually"), NULL,
    N_("Leave icons wherever they are dropped"),
    KRAKEN_FILE_SORT_NONE },
  { "Sort by Name", NULL,
    N_("By _Name"), NULL,
    N_("Keep icons sorted by name in rows"),
    KRAKEN_FILE_SORT_BY_DISPLAY_NAME },
  { "Sort by Size", NULL,
    N_("By _Size"), NULL,
    N_("Keep icons sorted by size in rows"),
    KRAKEN_FILE_SORT_BY_SIZE },
  { "Sort by Type", NULL,
    N_("By _Type"), NULL,
    N_("Keep icons sorted by type in rows"),
    KRAKEN_FILE_SORT_BY_TYPE },
  { "Sort by Detailed Type", NULL,
    N_("By _Detailed Type"), NULL,
    N_("Keep icons sorted by detailed type in rows"),
    KRAKEN_FILE_SORT_BY_DETAILED_TYPE },
  { "Sort by Modification Date", NULL,
    N_("By Modification _Date"), NULL,
    N_("Keep icons sorted by modification date in rows"),
    KRAKEN_FILE_SORT_BY_MTIME },
  { "Sort by Trash Time", NULL,
    N_("By T_rash Time"), NULL,
    N_("Keep icons sorted by trash time in rows"),
    KRAKEN_FILE_SORT_BY_TRASHED_TIME },
};

static void
kraken_icon_view_merge_menus (KrakenView *view)
{
	KrakenIconView *icon_view;
	GtkUIManager *ui_manager;
	GtkActionGroup *action_group;
	GtkAction *action;

        g_assert (KRAKEN_IS_ICON_VIEW (view));

	KRAKEN_VIEW_CLASS (kraken_icon_view_parent_class)->merge_menus (view);

	icon_view = KRAKEN_ICON_VIEW (view);

	ui_manager = kraken_view_get_ui_manager (KRAKEN_VIEW (icon_view));

	action_group = gtk_action_group_new ("IconViewActions");
	gtk_action_group_set_translation_domain (action_group, GETTEXT_PACKAGE);
	icon_view->details->icon_action_group = action_group;
	gtk_action_group_add_actions (action_group,
				      icon_view_entries, G_N_ELEMENTS (icon_view_entries),
				      icon_view);
	gtk_action_group_add_toggle_actions (action_group,
					     icon_view_toggle_entries, G_N_ELEMENTS (icon_view_toggle_entries),
					     icon_view);
	gtk_action_group_add_radio_actions (action_group,
					    arrange_radio_entries,
					    G_N_ELEMENTS (arrange_radio_entries),
					    -1,
					    G_CALLBACK (action_sort_radio_callback),
					    icon_view);

	gtk_ui_manager_insert_action_group (ui_manager, action_group, 0);
	g_object_unref (action_group); /* owned by ui manager */

	icon_view->details->icon_merge_id =
		gtk_ui_manager_add_ui_from_resource (ui_manager, "/org/kraken/kraken-icon-view-ui.xml", NULL);

	/* Do one-time state-setting here; context-dependent state-setting
	 * is done in update_menus.
	 */
	if (!kraken_icon_view_supports_auto_layout (icon_view)) {
		action = gtk_action_group_get_action (action_group,
						      KRAKEN_ACTION_ARRANGE_ITEMS);
		gtk_action_set_visible (action, FALSE);
	}

	update_layout_menus (icon_view);
}

static void
kraken_icon_view_unmerge_menus (KrakenView *view)
{
	KrakenIconView *icon_view;
	GtkUIManager *ui_manager;

	icon_view = KRAKEN_ICON_VIEW (view);

	KRAKEN_VIEW_CLASS (kraken_icon_view_parent_class)->unmerge_menus (view);

	ui_manager = kraken_view_get_ui_manager (view);
	if (ui_manager != NULL) {
		kraken_ui_unmerge_ui (ui_manager,
					&icon_view->details->icon_merge_id,
					&icon_view->details->icon_action_group);
	}
}

static void
kraken_icon_view_update_menus (KrakenView *view)
{
    KrakenIconView *icon_view;
    GtkAction *action;
    gboolean editable;

    icon_view = KRAKEN_ICON_VIEW (view);

	KRAKEN_VIEW_CLASS (kraken_icon_view_parent_class)->update_menus(view);

	editable = kraken_view_is_editable (view);
	action = gtk_action_group_get_action (icon_view->details->icon_action_group,
					      KRAKEN_ACTION_MANUAL_LAYOUT);
	gtk_action_set_sensitive (action, editable);
}

static void
kraken_icon_view_reset_to_defaults (KrakenView *view)
{
	KrakenIconContainer *icon_container;
	KrakenIconView *icon_view;

	icon_view = KRAKEN_ICON_VIEW (view);
	icon_container = get_icon_container (icon_view);

	clear_sort_criterion (icon_view);
	kraken_icon_container_set_keep_aligned
		(icon_container, get_default_directory_keep_aligned ());

	kraken_icon_container_sort (icon_container);

	update_layout_menus (icon_view);

	kraken_icon_view_restore_default_zoom_level (view);

    if (kraken_global_preferences_get_ignore_view_metadata ()) {
        KrakenWindow *window = kraken_view_get_kraken_window (view);
        kraken_window_set_ignore_meta_zoom_level (window, KRAKEN_ZOOM_LEVEL_NULL);
    }
}

static void
kraken_icon_view_select_all (KrakenView *view)
{
	KrakenIconContainer *icon_container;

	g_return_if_fail (KRAKEN_IS_ICON_VIEW (view));

	icon_container = get_icon_container (KRAKEN_ICON_VIEW (view));
        kraken_icon_container_select_all (icon_container);
}

static void
kraken_icon_view_reveal_selection (KrakenView *view)
{
	GList *selection;

	g_return_if_fail (KRAKEN_IS_ICON_VIEW (view));

        selection = kraken_view_get_selection (view);

	/* Make sure at least one of the selected items is scrolled into view */
	if (selection != NULL) {
		kraken_icon_container_reveal
			(get_icon_container (KRAKEN_ICON_VIEW (view)),
			 selection->data);
	}

        kraken_file_list_free (selection);
}

static GArray *
kraken_icon_view_get_selected_icon_locations (KrakenView *view)
{
	g_return_val_if_fail (KRAKEN_IS_ICON_VIEW (view), NULL);

	return kraken_icon_container_get_selected_icon_locations
		(get_icon_container (KRAKEN_ICON_VIEW (view)));
}


static void
kraken_icon_view_set_selection (KrakenView *view, GList *selection)
{
	g_return_if_fail (KRAKEN_IS_ICON_VIEW (view));

	kraken_icon_container_set_selection
		(get_icon_container (KRAKEN_ICON_VIEW (view)), selection);
}

static void
kraken_icon_view_invert_selection (KrakenView *view)
{
	g_return_if_fail (KRAKEN_IS_ICON_VIEW (view));

	kraken_icon_container_invert_selection
		(get_icon_container (KRAKEN_ICON_VIEW (view)));
}

static gboolean
kraken_icon_view_using_manual_layout (KrakenView *view)
{
	g_return_val_if_fail (KRAKEN_IS_ICON_VIEW (view), FALSE);

	return !kraken_icon_view_using_auto_layout (KRAKEN_ICON_VIEW (view));
}

static void
kraken_icon_view_widget_to_file_operation_position (KrakenView *view,
						GdkPoint *position)
{
	g_assert (KRAKEN_IS_ICON_VIEW (view));

	kraken_icon_container_widget_to_file_operation_position
		(get_icon_container (KRAKEN_ICON_VIEW (view)), position);
}

static void
icon_container_activate_callback (KrakenIconContainer *container,
				  GList *file_list,
				  KrakenIconView *icon_view)
{
	g_assert (KRAKEN_IS_ICON_VIEW (icon_view));
	g_assert (container == get_icon_container (icon_view));

	kraken_view_activate_files (KRAKEN_VIEW (icon_view),
				      file_list,
				      0, TRUE);
}

static void
icon_container_activate_previewer_callback (KrakenIconContainer *container,
					    GList *file_list,
					    GArray *locations,
					    KrakenIconView *icon_view)
{
	g_assert (KRAKEN_IS_ICON_VIEW (icon_view));
	g_assert (container == get_icon_container (icon_view));

	kraken_view_preview_files (KRAKEN_VIEW (icon_view),
				     file_list, locations);
}

/* this is called in one of these cases:
 * - we activate with enter holding shift
 * - we activate with space holding shift
 * - we double click an icon holding shift
 * - we middle click an icon
 *
 * If we don't open in new windows by default, the behavior should be
 * - middle click, shift + activate -> open in new tab
 * - shift + double click -> open in new window
 *
 * If we open in new windows by default, the behaviour should be
 * - middle click, or shift + activate, or shift + double-click -> close parent
 */
static void
icon_container_activate_alternate_callback (KrakenIconContainer *container,
					    GList *file_list,
					    KrakenIconView *icon_view)
{
	GdkEvent *event;
	GdkEventButton *button_event;
	GdkEventKey *key_event;
	gboolean open_in_tab, open_in_window, close_behind;
	KrakenWindowOpenFlags flags;

	g_assert (KRAKEN_IS_ICON_VIEW (icon_view));
	g_assert (container == get_icon_container (icon_view));

	flags = 0;
	event = gtk_get_current_event ();
	open_in_tab = FALSE;
	open_in_window = FALSE;
	close_behind = FALSE;

	if (g_settings_get_boolean (kraken_preferences,
				    KRAKEN_PREFERENCES_ALWAYS_USE_BROWSER)) {
		if (event->type == GDK_BUTTON_PRESS ||
		    event->type == GDK_BUTTON_RELEASE ||
		    event->type == GDK_2BUTTON_PRESS ||
		    event->type == GDK_3BUTTON_PRESS) {
			button_event = (GdkEventButton *) event;
			open_in_window = ((button_event->state & GDK_SHIFT_MASK) != 0);
			open_in_tab = !open_in_window;
		} else if (event->type == GDK_KEY_PRESS ||
			   event->type == GDK_KEY_RELEASE) {
			key_event = (GdkEventKey *) event;
			open_in_tab = ((key_event->state & GDK_SHIFT_MASK) != 0);
		}
	} else {
		close_behind = TRUE;
	}

	if (open_in_tab) {
		flags |= KRAKEN_WINDOW_OPEN_FLAG_NEW_TAB;
	}

	if (open_in_window) {
		flags |= KRAKEN_WINDOW_OPEN_FLAG_NEW_WINDOW;
	}

	if (close_behind) {
		flags |= KRAKEN_WINDOW_OPEN_FLAG_CLOSE_BEHIND;
	}

	DEBUG ("Activate alternate, open in tab %d, close behind %d, new window %d\n",
	       open_in_tab, close_behind, open_in_window);

	kraken_view_activate_files (KRAKEN_VIEW (icon_view),
				      file_list,
				      flags,
				      TRUE);
}

static void
band_select_started_callback (KrakenIconContainer *container,
			      KrakenIconView *icon_view)
{
	g_assert (KRAKEN_IS_ICON_VIEW (icon_view));
	g_assert (container == get_icon_container (icon_view));

	kraken_view_start_batching_selection_changes (KRAKEN_VIEW (icon_view));
}

static void
band_select_ended_callback (KrakenIconContainer *container,
			    KrakenIconView *icon_view)
{
	g_assert (KRAKEN_IS_ICON_VIEW (icon_view));
	g_assert (container == get_icon_container (icon_view));

	kraken_view_stop_batching_selection_changes (KRAKEN_VIEW (icon_view));
}

int
kraken_icon_view_compare_files (KrakenIconView   *icon_view,
				  KrakenFile *a,
				  KrakenFile *b)
{
	return kraken_file_compare_for_sort
		(a, b, icon_view->details->sort->sort_type,
		 /* Use type-unsafe cast for performance */
		 kraken_view_should_sort_directories_first ((KrakenView *)icon_view),
		 kraken_view_should_sort_favorites_first ((KrakenView *)icon_view),
		 icon_view->details->sort_reversed,
         NULL);
}

static int
compare_files (KrakenView   *icon_view,
	       KrakenFile *a,
	       KrakenFile *b)
{
	return kraken_icon_view_compare_files ((KrakenIconView *)icon_view, a, b);
}

static void
kraken_icon_view_screen_changed (GtkWidget *widget,
				   GdkScreen *previous_screen)
{
	KrakenView *view;
	GList *files, *l;
	KrakenFile *file;
	KrakenDirectory *directory;
	KrakenIconContainer *icon_container;

	if (GTK_WIDGET_CLASS (kraken_icon_view_parent_class)->screen_changed) {
		GTK_WIDGET_CLASS (kraken_icon_view_parent_class)->screen_changed (widget, previous_screen);
	}

	view = KRAKEN_VIEW (widget);
	if (KRAKEN_ICON_VIEW (view)->details->is_desktop) {
		icon_container = get_icon_container (KRAKEN_ICON_VIEW (view));

		directory = kraken_view_get_model (view);
		files = kraken_directory_get_file_list (directory);

		for (l = files; l != NULL; l = l->next) {
			file = l->data;

			if (!should_show_file_on_screen (view, file)) {
				kraken_icon_view_remove_file (view, file, directory);
			} else {
				if (kraken_icon_container_add (icon_container,
								 KRAKEN_ICON_CONTAINER_ICON_DATA (file))) {
					kraken_file_ref (file);
				}
			}
		}

		kraken_file_list_unref (files);
		g_list_free (files);
	}
}

static gboolean
kraken_icon_view_scroll_event (GtkWidget *widget,
			   GdkEventScroll *scroll_event)
{
	KrakenIconView *icon_view;
	GdkEvent *event_copy;
	GdkEventScroll *scroll_event_copy;
	gboolean ret;

	icon_view = KRAKEN_ICON_VIEW (widget);

	if (icon_view->details->compact &&
	    (scroll_event->direction == GDK_SCROLL_UP ||
	     scroll_event->direction == GDK_SCROLL_DOWN ||
	     scroll_event->direction == GDK_SCROLL_SMOOTH)) {
		ret = kraken_view_handle_scroll_event (KRAKEN_VIEW (icon_view), scroll_event);
		if (!ret) {
			/* in column-wise layout, re-emit vertical mouse scroll events as horizontal ones,
			 * if they don't bump zoom */
			event_copy = gdk_event_copy ((GdkEvent *) scroll_event);
			scroll_event_copy = (GdkEventScroll *) event_copy;

			/* transform vertical integer smooth scroll events into horizontal events */
			if (scroll_event_copy->direction == GDK_SCROLL_SMOOTH &&
				   scroll_event_copy->delta_x == 0) {
				if (scroll_event_copy->delta_y == 1.0) {
					scroll_event_copy->direction = GDK_SCROLL_DOWN;
				} else if (scroll_event_copy->delta_y == -1.0) {
					scroll_event_copy->direction = GDK_SCROLL_UP;
				}
			}

			if (scroll_event_copy->direction == GDK_SCROLL_UP) {
				scroll_event_copy->direction = GDK_SCROLL_LEFT;
			} else if (scroll_event_copy->direction == GDK_SCROLL_DOWN) {
				scroll_event_copy->direction = GDK_SCROLL_RIGHT;
			}

			ret = GTK_WIDGET_CLASS (kraken_icon_view_parent_class)->scroll_event (widget, scroll_event_copy);
			gdk_event_free (event_copy);
		}

		return ret;
	}

	return GTK_WIDGET_CLASS (kraken_icon_view_parent_class)->scroll_event (widget, scroll_event);
}

static void
selection_changed_callback (KrakenIconContainer *container,
			    KrakenIconView *icon_view)
{
	g_assert (KRAKEN_IS_ICON_VIEW (icon_view));
	g_assert (container == get_icon_container (icon_view));

	kraken_view_notify_selection_changed (KRAKEN_VIEW (icon_view));
}

static void
icon_container_context_click_selection_callback (KrakenIconContainer *container,
						 GdkEventButton *event,
						 KrakenIconView *icon_view)
{
	g_assert (KRAKEN_IS_ICON_CONTAINER (container));
	g_assert (KRAKEN_IS_ICON_VIEW (icon_view));

	kraken_view_pop_up_selection_context_menu
		(KRAKEN_VIEW (icon_view), event);
}

static void
icon_container_context_click_background_callback (KrakenIconContainer *container,
						  GdkEventButton *event,
						  KrakenIconView *icon_view)
{
	g_assert (KRAKEN_IS_ICON_CONTAINER (container));
	g_assert (KRAKEN_IS_ICON_VIEW (icon_view));

	kraken_view_pop_up_background_context_menu
		(KRAKEN_VIEW (icon_view), event);
}

static gboolean
kraken_icon_view_react_to_icon_change_idle_callback (gpointer data)
{
        KrakenIconView *icon_view;

        g_assert (KRAKEN_IS_ICON_VIEW (data));

        icon_view = KRAKEN_ICON_VIEW (data);
        icon_view->details->react_to_icon_change_idle_id = 0;

	/* Rebuild the menus since some of them (e.g. Restore Stretched Icons)
	 * may be different now.
	 */
	kraken_view_update_menus (KRAKEN_VIEW (icon_view));

        /* Don't call this again (unless rescheduled) */
        return FALSE;
}

static void
icon_position_changed_callback (KrakenIconContainer *container,
				KrakenFile *file,
				const KrakenIconPosition *position,
				KrakenIconView *icon_view)
{
	char scale_string[G_ASCII_DTOSTR_BUF_SIZE];

	g_assert (KRAKEN_IS_ICON_VIEW (icon_view));
	g_assert (container == get_icon_container (icon_view));
	g_assert (KRAKEN_IS_FILE (file));

	/* Schedule updating menus for the next idle. Doing it directly here
	 * noticeably slows down icon stretching.  The other work here to
	 * store the icon position and scale does not seem to noticeably
	 * slow down icon stretching. It would be trickier to move to an
	 * idle call, because we'd have to keep track of potentially multiple
	 * sets of file/geometry info.
	 */
	if (kraken_view_get_active (KRAKEN_VIEW (icon_view)) &&
	    icon_view->details->react_to_icon_change_idle_id == 0) {
                icon_view->details->react_to_icon_change_idle_id
                        = g_idle_add (kraken_icon_view_react_to_icon_change_idle_callback,
				      icon_view);
	}

	/* Store the new position of the icon in the metadata. */
	if (!kraken_file_get_is_desktop_orphan (file)) {
		kraken_file_set_position (file, position->x, position->y);
        kraken_file_set_monitor_number (file, position->monitor);
	}

	g_ascii_dtostr (scale_string, sizeof (scale_string), position->scale);

    sync_directory_monitor_number (icon_view, file);

	kraken_file_set_metadata (file, KRAKEN_METADATA_KEY_ICON_SCALE, "1.0", scale_string);
}

/* Attempt to change the filename to the new text.  Notify user if operation fails. */
static void
icon_rename_ended_cb (KrakenIconContainer *container,
		      KrakenFile *file,
		      const char *new_name,
		      KrakenIconView *icon_view)
{
	g_assert (KRAKEN_IS_FILE (file));

	kraken_view_set_is_renaming (KRAKEN_VIEW (icon_view), FALSE);

	/* Don't allow a rename with an empty string. Revert to original
	 * without notifying the user.
	 */
	if ((new_name == NULL) || (new_name[0] == '\0')) {
		return;
	}

	kraken_rename_file (file, new_name, NULL, NULL);
}

static void
icon_rename_started_cb (KrakenIconContainer *container,
			GtkWidget *widget,
			gpointer callback_data)
{
	KrakenView *directory_view;

	directory_view = KRAKEN_VIEW (callback_data);
	kraken_clipboard_set_up_editable
		(GTK_EDITABLE (widget),
		 kraken_view_get_ui_manager (directory_view),
		 FALSE);
}

static char *
get_icon_uri_callback (KrakenIconContainer *container,
		       KrakenFile *file,
		       KrakenIconView *icon_view)
{
	g_assert (KRAKEN_IS_ICON_CONTAINER (container));
	g_assert (KRAKEN_IS_FILE (file));
	g_assert (KRAKEN_IS_ICON_VIEW (icon_view));

	return kraken_file_get_local_uri (file);
}

static char *
get_icon_drop_target_uri_callback (KrakenIconContainer *container,
		       		   KrakenFile *file,
		       		   KrakenIconView *icon_view)
{
	g_return_val_if_fail (KRAKEN_IS_ICON_CONTAINER (container), NULL);
	g_return_val_if_fail (KRAKEN_IS_FILE (file), NULL);
	g_return_val_if_fail (KRAKEN_IS_ICON_VIEW (icon_view), NULL);

	return kraken_file_get_drop_target_uri (file);
}

/* Preferences changed callbacks */
static void
kraken_icon_view_click_policy_changed (KrakenView *directory_view)
{
    g_assert (KRAKEN_IS_ICON_VIEW (directory_view));

    kraken_icon_view_update_click_mode (KRAKEN_ICON_VIEW (directory_view));
}

static void
kraken_icon_view_click_to_rename_mode_changed (KrakenView *directory_view)
{
    g_assert (KRAKEN_IS_ICON_VIEW (directory_view));

    kraken_icon_view_update_click_to_rename_mode (KRAKEN_ICON_VIEW (directory_view));
}

static void
image_display_policy_changed_callback (gpointer callback_data)
{
	KrakenIconView *icon_view;

	icon_view = KRAKEN_ICON_VIEW (callback_data);

	kraken_icon_container_request_update_all (get_icon_container (icon_view));
}

static void
text_attribute_names_changed_callback (gpointer callback_data)
{
	KrakenIconView *icon_view;

	icon_view = KRAKEN_ICON_VIEW (callback_data);

	kraken_icon_container_request_update_all (get_icon_container (icon_view));
}

static void
default_sort_order_changed_callback (gpointer callback_data)
{
	KrakenIconView *icon_view;
	KrakenFile *file;
	char *sort_name;
	KrakenIconContainer *icon_container;

	g_return_if_fail (KRAKEN_IS_ICON_VIEW (callback_data));

	icon_view = KRAKEN_ICON_VIEW (callback_data);

	file = kraken_view_get_directory_as_file (KRAKEN_VIEW (icon_view));
	sort_name = kraken_icon_view_get_directory_sort_by (icon_view, file);
	set_sort_criterion (icon_view, get_sort_criterion_by_metadata_text (sort_name), FALSE);
	g_free (sort_name);

	icon_container = get_icon_container (icon_view);
	g_return_if_fail (KRAKEN_IS_ICON_CONTAINER (icon_container));

	kraken_icon_container_request_update_all (icon_container);
}

static void
default_sort_in_reverse_order_changed_callback (gpointer callback_data)
{
	KrakenIconView *icon_view;
	KrakenFile *file;
	KrakenIconContainer *icon_container;

	g_return_if_fail (KRAKEN_IS_ICON_VIEW (callback_data));

	icon_view = KRAKEN_ICON_VIEW (callback_data);

	file = kraken_view_get_directory_as_file (KRAKEN_VIEW (icon_view));
	kraken_icon_view_set_sort_reversed (icon_view, kraken_icon_view_get_directory_sort_reversed (icon_view, file), FALSE);
	icon_container = get_icon_container (icon_view);
	g_return_if_fail (KRAKEN_IS_ICON_CONTAINER (icon_container));

	kraken_icon_container_request_update_all (icon_container);
}

static void
default_zoom_level_changed_callback (gpointer callback_data)
{
	KrakenIconView *icon_view;
	KrakenFile *file;
	int level;

	g_return_if_fail (KRAKEN_IS_ICON_VIEW (callback_data));

	icon_view = KRAKEN_ICON_VIEW (callback_data);

	if (kraken_view_supports_zooming (KRAKEN_VIEW (icon_view))) {
		file = kraken_view_get_directory_as_file (KRAKEN_VIEW (icon_view));

        if (kraken_global_preferences_get_ignore_view_metadata () &&
            kraken_window_get_ignore_meta_zoom_level (kraken_view_get_kraken_window (KRAKEN_VIEW (icon_view))) > -1) {
            level = kraken_window_get_ignore_meta_zoom_level (kraken_view_get_kraken_window (KRAKEN_VIEW (icon_view)));
        } else {
            sync_directory_monitor_number (icon_view, file);

            if (kraken_icon_view_is_compact (icon_view)) {
                level = kraken_file_get_integer_metadata (file,
                                                        KRAKEN_METADATA_KEY_COMPACT_VIEW_ZOOM_LEVEL,
                                                        get_default_zoom_level (icon_view));
            } else {
                level = kraken_file_get_integer_metadata (file,
                                                        KRAKEN_METADATA_KEY_ICON_VIEW_ZOOM_LEVEL,
                                                        get_default_zoom_level (icon_view));
            }
        }
        kraken_view_zoom_to_level (KRAKEN_VIEW (icon_view), level);
    }
}

static void
labels_beside_icons_changed_callback (gpointer callback_data)
{
	KrakenIconView *icon_view;

	g_return_if_fail (KRAKEN_IS_ICON_VIEW (callback_data));

	icon_view = KRAKEN_ICON_VIEW (callback_data);

	set_labels_beside_icons (icon_view);
}

static void
all_columns_same_width_changed_callback (gpointer callback_data)
{
	KrakenIconView *icon_view;

	g_assert (KRAKEN_IS_ICON_VIEW (callback_data));

	icon_view = KRAKEN_ICON_VIEW (callback_data);

	set_columns_same_width (icon_view);
}


static void
kraken_icon_view_sort_directories_first_changed (KrakenView *directory_view)
{
	KrakenIconView *icon_view;

	icon_view = KRAKEN_ICON_VIEW (directory_view);

	if (kraken_icon_view_using_auto_layout (icon_view)) {
		kraken_icon_container_sort
			(get_icon_container (icon_view));
	}
}

static void
kraken_icon_view_sort_favorites_first_changed (KrakenView *directory_view)
{
	KrakenIconView *icon_view;

	icon_view = KRAKEN_ICON_VIEW (directory_view);

	if (kraken_icon_view_using_auto_layout (icon_view)) {
		kraken_icon_container_sort
			(get_icon_container (icon_view));
	}
}

static gboolean
icon_view_can_accept_item (KrakenIconContainer *container,
			   KrakenFile *target_item,
			   const char *item_uri,
			   KrakenView *view)
{
	return kraken_drag_can_accept_item (target_item, item_uri);
}

static char *
icon_view_get_container_uri (KrakenIconContainer *container,
			     KrakenView *view)
{
	return kraken_view_get_uri (view);
}

static void
icon_view_move_copy_items (KrakenIconContainer *container,
			   const GList *item_uris,
			   GArray *relative_item_points,
			   const char *target_dir,
			   int copy_action,
			   int x, int y,
			   KrakenView *view)
{
	kraken_clipboard_clear_if_colliding_uris (GTK_WIDGET (view),
						    item_uris,
						    kraken_view_get_copied_files_atom (view));
	kraken_view_move_copy_items (view, item_uris, relative_item_points, target_dir,
				       copy_action, x, y);
}

static void
kraken_icon_view_update_click_mode (KrakenIconView *icon_view)
{
	KrakenIconContainer	*icon_container;
	int			click_mode;

	icon_container = get_icon_container (icon_view);
	g_assert (icon_container != NULL);

	click_mode = g_settings_get_enum (kraken_preferences, KRAKEN_PREFERENCES_CLICK_POLICY);

	kraken_icon_container_set_single_click_mode (icon_container,
						       click_mode == KRAKEN_CLICK_POLICY_SINGLE);
}

static void
kraken_icon_view_update_click_to_rename_mode (KrakenIconView *icon_view)
{
    KrakenIconContainer   *icon_container;
    gboolean enabled;

    icon_container = get_icon_container (icon_view);
    g_assert (icon_container != NULL);

    enabled = g_settings_get_boolean (kraken_preferences, KRAKEN_PREFERENCES_CLICK_TO_RENAME);

    kraken_icon_container_set_click_to_rename_enabled (icon_container,
                                                     enabled);
}

static gboolean
get_stored_layout_timestamp (KrakenIconContainer *container,
			     KrakenIconData *icon_data,
			     time_t *timestamp,
			     KrakenIconView *view)
{
	KrakenFile *file;
	KrakenDirectory *directory;

	if (icon_data == NULL) {
		directory = kraken_view_get_model (KRAKEN_VIEW (view));
		if (directory == NULL) {
			return FALSE;
		}

		file = kraken_directory_get_corresponding_file (directory);

        sync_directory_monitor_number (view, file);

        *timestamp = kraken_file_get_time_metadata (file, KRAKEN_METADATA_KEY_ICON_VIEW_LAYOUT_TIMESTAMP);

		kraken_file_unref (file);
	} else {
        sync_directory_monitor_number (view, KRAKEN_FILE (icon_data));

        *timestamp = kraken_file_get_time_metadata (KRAKEN_FILE (icon_data), KRAKEN_METADATA_KEY_ICON_POSITION_TIMESTAMP);
	}

	return TRUE;
}

static gboolean
store_layout_timestamp (KrakenIconContainer *container,
			KrakenIconData *icon_data,
			const time_t *timestamp,
			KrakenIconView *view)
{
	KrakenFile *file;
	KrakenDirectory *directory;

	if (icon_data == NULL) {
		directory = kraken_view_get_model (KRAKEN_VIEW (view));
		if (directory == NULL) {
			return FALSE;
		}

		file = kraken_directory_get_corresponding_file (directory);

        sync_directory_monitor_number (view, file);

		kraken_file_set_time_metadata (file,
                                     KRAKEN_METADATA_KEY_ICON_VIEW_LAYOUT_TIMESTAMP,
                                     (time_t) *timestamp);
		kraken_file_unref (file);
	} else {
        sync_directory_monitor_number (view, KRAKEN_FILE (icon_data));

		kraken_file_set_time_metadata (KRAKEN_FILE (icon_data),
                                     KRAKEN_METADATA_KEY_ICON_POSITION_TIMESTAMP,
                                     (time_t) *timestamp);
	}

	return TRUE;
}

static gboolean
focus_in_event_callback (GtkWidget *widget, GdkEventFocus *event, gpointer user_data)
{
	KrakenWindowSlot *slot;
	KrakenIconView *icon_view = KRAKEN_ICON_VIEW (user_data);

	/* make the corresponding slot (and the pane that contains it) active */
	slot = kraken_view_get_kraken_window_slot (KRAKEN_VIEW (icon_view));
	kraken_window_slot_make_hosting_pane_active (slot);

	return FALSE;
}

static gboolean
button_press_callback (GtkWidget *widget, GdkEventFocus *event, gpointer user_data)
{
    KrakenView *view = KRAKEN_VIEW (user_data);
    GdkEventButton *event_button = (GdkEventButton *)event;
    gint selection_count = kraken_view_get_selection_count (KRAKEN_VIEW (view));

    if (!kraken_view_get_active (view) && selection_count > 0) {
        KrakenWindowSlot *slot = kraken_view_get_kraken_window_slot (view);
        kraken_window_slot_make_hosting_pane_active (slot);
        return GDK_EVENT_STOP;
    }

    /* double left click on blank will go to parent folder */
    if (g_settings_get_boolean (kraken_preferences, KRAKEN_PREFERENCES_CLICK_DOUBLE_PARENT_FOLDER) &&
        (event_button->button == 1) && (event_button->type == GDK_2BUTTON_PRESS)) {
        if (selection_count == 0) {
            KrakenWindowSlot *slot = kraken_view_get_kraken_window_slot (view);
            kraken_window_slot_go_up (slot, 0);
            return GDK_EVENT_STOP;
        }
    }

    return GDK_EVENT_PROPAGATE;
}

static KrakenIconContainer *
create_icon_container (KrakenIconView *icon_view)
{
	KrakenIconContainer *icon_container;

    if (KRAKEN_ICON_VIEW_GET_CLASS (icon_view)->use_grid_container) {
        icon_container = kraken_icon_view_grid_container_new (icon_view,
                                                            icon_view->details->is_desktop);
    } else {
        icon_container = kraken_icon_view_container_new (icon_view,
                                                       icon_view->details->is_desktop);
    }

	icon_view->details->icon_container = GTK_WIDGET (icon_container);
	g_object_add_weak_pointer (G_OBJECT (icon_container),
				   (gpointer *) &icon_view->details->icon_container);

	gtk_widget_set_can_focus (GTK_WIDGET (icon_container), TRUE);

    g_signal_connect_object (icon_container, "button_press_event",
                 G_CALLBACK (button_press_callback), icon_view, 0);
	g_signal_connect_object (icon_container, "focus_in_event",
				 G_CALLBACK (focus_in_event_callback), icon_view, 0);
	g_signal_connect_object (icon_container, "activate",
				 G_CALLBACK (icon_container_activate_callback), icon_view, 0);
	g_signal_connect_object (icon_container, "activate_alternate",
				 G_CALLBACK (icon_container_activate_alternate_callback), icon_view, 0);
	g_signal_connect_object (icon_container, "activate_previewer",
				 G_CALLBACK (icon_container_activate_previewer_callback), icon_view, 0);
	g_signal_connect_object (icon_container, "band_select_started",
				 G_CALLBACK (band_select_started_callback), icon_view, 0);
	g_signal_connect_object (icon_container, "band_select_ended",
				 G_CALLBACK (band_select_ended_callback), icon_view, 0);
	g_signal_connect_object (icon_container, "context_click_selection",
				 G_CALLBACK (icon_container_context_click_selection_callback), icon_view, 0);
	g_signal_connect_object (icon_container, "context_click_background",
				 G_CALLBACK (icon_container_context_click_background_callback), icon_view, 0);
	g_signal_connect_object (icon_container, "icon_position_changed",
				 G_CALLBACK (icon_position_changed_callback), icon_view, 0);
	g_signal_connect_object (icon_container, "selection_changed",
				 G_CALLBACK (selection_changed_callback), icon_view, 0);
	/* FIXME: many of these should move into fm-icon-container as virtual methods */
	g_signal_connect_object (icon_container, "get_icon_uri",
				 G_CALLBACK (get_icon_uri_callback), icon_view, 0);
	g_signal_connect_object (icon_container, "get_icon_drop_target_uri",
				 G_CALLBACK (get_icon_drop_target_uri_callback), icon_view, 0);
	g_signal_connect_object (icon_container, "move_copy_items",
				 G_CALLBACK (icon_view_move_copy_items), icon_view, 0);
	g_signal_connect_object (icon_container, "get_container_uri",
				 G_CALLBACK (icon_view_get_container_uri), icon_view, 0);
	g_signal_connect_object (icon_container, "can_accept_item",
				 G_CALLBACK (icon_view_can_accept_item), icon_view, 0);
	g_signal_connect_object (icon_container, "layout_changed",
				 G_CALLBACK (layout_changed_callback), icon_view, 0);
	g_signal_connect_object (icon_container, "icon_rename_started",
				 G_CALLBACK (icon_rename_started_cb), icon_view, 0);
	g_signal_connect_object (icon_container, "icon_rename_ended",
				 G_CALLBACK (icon_rename_ended_cb), icon_view, 0);
	g_signal_connect_object (icon_container, "icon_stretch_started",
				 G_CALLBACK (kraken_view_update_menus), icon_view,
				 G_CONNECT_SWAPPED);
	g_signal_connect_object (icon_container, "icon_stretch_ended",
				 G_CALLBACK (kraken_view_update_menus), icon_view,
				 G_CONNECT_SWAPPED);

	g_signal_connect_object (icon_container, "get_stored_layout_timestamp",
				 G_CALLBACK (get_stored_layout_timestamp), icon_view, 0);
	g_signal_connect_object (icon_container, "store_layout_timestamp",
				 G_CALLBACK (store_layout_timestamp), icon_view, 0);

	gtk_container_add (GTK_CONTAINER (icon_view),
			   GTK_WIDGET (icon_container));

	kraken_icon_view_update_click_mode (icon_view);
    kraken_icon_view_update_click_to_rename_mode (icon_view);

	gtk_widget_show (GTK_WIDGET (icon_container));

	return icon_container;
}

/* Handles an URL received from Mozilla */
static void
icon_view_handle_netscape_url (KrakenIconContainer *container, const char *encoded_url,
			       const char *target_uri,
			       GdkDragAction action, int x, int y, KrakenIconView *view)
{
	kraken_view_handle_netscape_url_drop (KRAKEN_VIEW (view),
						encoded_url, target_uri, action, x, y);
}

static void
icon_view_handle_uri_list (KrakenIconContainer *container, const char *item_uris,
			   const char *target_uri,
			   GdkDragAction action, int x, int y, KrakenIconView *view)
{
	kraken_view_handle_uri_list_drop (KRAKEN_VIEW (view),
					    item_uris, target_uri, action, x, y);
}

static void
icon_view_handle_text (KrakenIconContainer *container, const char *text,
		       const char *target_uri,
		       GdkDragAction action, int x, int y, KrakenIconView *view)
{
	kraken_view_handle_text_drop (KRAKEN_VIEW (view),
					text, target_uri, action, x, y);
}

static void
icon_view_handle_raw (KrakenIconContainer *container, const char *raw_data,
		      int length, const char *target_uri, const char *direct_save_uri,
		      GdkDragAction action, int x, int y, KrakenIconView *view)
{
	kraken_view_handle_raw_drop (KRAKEN_VIEW (view),
				       raw_data, length, target_uri, direct_save_uri, action, x, y);
}

static char *
icon_view_get_first_visible_file (KrakenView *view)
{
	KrakenFile *file;
	KrakenIconView *icon_view;

	icon_view = KRAKEN_ICON_VIEW (view);

	file = KRAKEN_FILE (kraken_icon_container_get_first_visible_icon (get_icon_container (icon_view)));

	if (file) {
		return kraken_file_get_uri (file);
	}

	return NULL;
}

static void
icon_view_scroll_to_file (KrakenView *view,
			  const char *uri)
{
	KrakenFile *file;
	KrakenIconView *icon_view;

	icon_view = KRAKEN_ICON_VIEW (view);

	if (uri != NULL) {
		/* Only if existing, since we don't want to add the file to
		   the directory if it has been removed since then */
		file = kraken_file_get_existing_by_uri (uri);
		if (file != NULL) {
			kraken_icon_container_scroll_to_icon (get_icon_container (icon_view),
								KRAKEN_ICON_CONTAINER_ICON_DATA (file));
			kraken_file_unref (file);
		}
	}
}

static const char *
kraken_icon_view_get_id (KrakenView *view)
{
	if (kraken_icon_view_is_compact (KRAKEN_ICON_VIEW (view))) {
		return FM_COMPACT_VIEW_ID;
	}

	return KRAKEN_ICON_VIEW_ID;
}

static void
set_compact_view (KrakenIconView *icon_view,
                  gboolean      compact)
{
    icon_view->details->compact = compact;

    if (compact) {
        kraken_icon_container_set_layout_mode (get_icon_container (icon_view),
                                             gtk_widget_get_direction (GTK_WIDGET(icon_view)) == GTK_TEXT_DIR_RTL ?
                                                                                                     KRAKEN_ICON_LAYOUT_T_B_R_L :
                                                                                                     KRAKEN_ICON_LAYOUT_T_B_L_R);
        kraken_icon_container_set_forced_icon_size (get_icon_container (icon_view),
                                                  KRAKEN_COMPACT_FORCED_ICON_SIZE);
    } else {
        kraken_icon_container_set_layout_mode (get_icon_container (icon_view),
                                             gtk_widget_get_direction (GTK_WIDGET(icon_view)) == GTK_TEXT_DIR_RTL ?
                                                                                                     KRAKEN_ICON_LAYOUT_R_L_T_B :
                                                                                                     KRAKEN_ICON_LAYOUT_L_R_T_B);
        kraken_icon_container_set_forced_icon_size (get_icon_container (icon_view),
                                                  0);
    }
}

static void
kraken_icon_view_set_property (GObject         *object,
			   guint            prop_id,
			   const GValue    *value,
			   GParamSpec      *pspec)
{
	KrakenIconView *icon_view;

	icon_view = KRAKEN_ICON_VIEW (object);

	switch (prop_id)  {
	case PROP_COMPACT:
        set_compact_view (icon_view, g_value_get_boolean (value));
		break;
	case PROP_SUPPORTS_AUTO_LAYOUT:
		icon_view->details->supports_auto_layout = g_value_get_boolean (value);
		break;
	case PROP_IS_DESKTOP:
		icon_view->details->is_desktop = g_value_get_boolean (value);
		break;
	case PROP_SUPPORTS_KEEP_ALIGNED:
		icon_view->details->supports_keep_aligned = g_value_get_boolean (value);
		break;
	case PROP_SUPPORTS_LABELS_BESIDE_ICONS:
		icon_view->details->supports_labels_beside_icons = g_value_get_boolean (value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
kraken_icon_view_finalize (GObject *object)
{
	KrakenIconView *icon_view;

	icon_view = KRAKEN_ICON_VIEW (object);

	g_free (icon_view->details);

	g_signal_handlers_disconnect_by_func (kraken_preferences,
					      default_sort_order_changed_callback,
					      icon_view);
	g_signal_handlers_disconnect_by_func (kraken_preferences,
					      default_sort_in_reverse_order_changed_callback,
					      icon_view);
	g_signal_handlers_disconnect_by_func (kraken_preferences,
					      image_display_policy_changed_callback,
					      icon_view);

	g_signal_handlers_disconnect_by_func (kraken_icon_view_preferences,
					      default_zoom_level_changed_callback,
					      icon_view);
	g_signal_handlers_disconnect_by_func (kraken_icon_view_preferences,
					      labels_beside_icons_changed_callback,
					      icon_view);
	g_signal_handlers_disconnect_by_func (kraken_icon_view_preferences,
					      text_attribute_names_changed_callback,
					      icon_view);

	g_signal_handlers_disconnect_by_func (kraken_compact_view_preferences,
					      default_zoom_level_changed_callback,
					      icon_view);
	g_signal_handlers_disconnect_by_func (kraken_compact_view_preferences,
					      all_columns_same_width_changed_callback,
					      icon_view);

	G_OBJECT_CLASS (kraken_icon_view_parent_class)->finalize (object);
}

static void
kraken_icon_view_constructed (GObject *object)
{
    KrakenIconView *icon_view;
    KrakenIconContainer *icon_container;

    icon_view = KRAKEN_ICON_VIEW (object);

    G_OBJECT_CLASS (kraken_icon_view_parent_class)->constructed (G_OBJECT (icon_view));

    g_return_if_fail (gtk_bin_get_child (GTK_BIN (icon_view)) == NULL);

    icon_container = create_icon_container (icon_view);

    /* Set our default layout mode */
    if (!icon_view->details->is_desktop) {
        kraken_icon_container_set_layout_mode (icon_container,
                                             gtk_widget_get_direction (GTK_WIDGET(icon_container)) == GTK_TEXT_DIR_RTL ?
                                             KRAKEN_ICON_LAYOUT_R_L_T_B :
                                             KRAKEN_ICON_LAYOUT_L_R_T_B);
    }

    g_signal_connect_swapped (kraken_preferences,
                  "changed::" KRAKEN_PREFERENCES_DEFAULT_SORT_ORDER,
                  G_CALLBACK (default_sort_order_changed_callback),
                  icon_view);
    g_signal_connect_swapped (kraken_preferences,
                  "changed::" KRAKEN_PREFERENCES_DEFAULT_SORT_IN_REVERSE_ORDER,
                  G_CALLBACK (default_sort_in_reverse_order_changed_callback),
                  icon_view);
    g_signal_connect_swapped (kraken_preferences,
                  "changed::" KRAKEN_PREFERENCES_SHOW_IMAGE_FILE_THUMBNAILS,
                  G_CALLBACK (image_display_policy_changed_callback),
                  icon_view);
    g_signal_connect_swapped (kraken_icon_view_preferences,
                  "changed::" KRAKEN_PREFERENCES_ICON_VIEW_DEFAULT_ZOOM_LEVEL,
                  G_CALLBACK (default_zoom_level_changed_callback),
                  icon_view);
    g_signal_connect_swapped (kraken_icon_view_preferences,
                  "changed::" KRAKEN_PREFERENCES_ICON_VIEW_LABELS_BESIDE_ICONS,
                  G_CALLBACK (labels_beside_icons_changed_callback),
                  icon_view);
    g_signal_connect_swapped (kraken_icon_view_preferences,
                  "changed::" KRAKEN_PREFERENCES_ICON_VIEW_CAPTIONS,
                  G_CALLBACK (text_attribute_names_changed_callback),
                  icon_view);

    g_signal_connect_swapped (kraken_compact_view_preferences,
                  "changed::" KRAKEN_PREFERENCES_COMPACT_VIEW_DEFAULT_ZOOM_LEVEL,
                  G_CALLBACK (default_zoom_level_changed_callback),
                  icon_view);
    g_signal_connect_swapped (kraken_compact_view_preferences,
                  "changed::" KRAKEN_PREFERENCES_COMPACT_VIEW_ALL_COLUMNS_SAME_WIDTH,
                  G_CALLBACK (all_columns_same_width_changed_callback),
                  icon_view);

    g_signal_connect_object (get_icon_container (icon_view), "handle_netscape_url",
                 G_CALLBACK (icon_view_handle_netscape_url), icon_view, 0);
    g_signal_connect_object (get_icon_container (icon_view), "handle_uri_list",
                 G_CALLBACK (icon_view_handle_uri_list), icon_view, 0);
    g_signal_connect_object (get_icon_container (icon_view), "handle_text",
                 G_CALLBACK (icon_view_handle_text), icon_view, 0);
    g_signal_connect_object (get_icon_container (icon_view), "handle_raw",
                 G_CALLBACK (icon_view_handle_raw), icon_view, 0);

    icon_view->details->clipboard_handler_id =
        g_signal_connect (kraken_clipboard_monitor_get (),
                          "clipboard_info",
                          G_CALLBACK (icon_view_notify_clipboard_info), icon_view);

    kraken_icon_container_set_is_desktop (icon_container, FALSE);
}

static void
kraken_icon_view_class_init (KrakenIconViewClass *klass)
{
	KrakenViewClass *kraken_view_class;
	GObjectClass *oclass;

    klass->use_grid_container = FALSE;

	kraken_view_class = KRAKEN_VIEW_CLASS (klass);
	oclass = G_OBJECT_CLASS (klass);

	oclass->set_property = kraken_icon_view_set_property;
	oclass->finalize = kraken_icon_view_finalize;
    oclass->constructed = kraken_icon_view_constructed;

	GTK_WIDGET_CLASS (klass)->destroy = kraken_icon_view_destroy;
	GTK_WIDGET_CLASS (klass)->screen_changed = kraken_icon_view_screen_changed;
	GTK_WIDGET_CLASS (klass)->scroll_event = kraken_icon_view_scroll_event;

	kraken_view_class->add_file = kraken_icon_view_add_file;
	kraken_view_class->begin_loading = kraken_icon_view_begin_loading;
	kraken_view_class->bump_zoom_level = kraken_icon_view_bump_zoom_level;
	kraken_view_class->can_rename_file = kraken_icon_view_can_rename_file;
	kraken_view_class->can_zoom_in = kraken_icon_view_can_zoom_in;
	kraken_view_class->can_zoom_out = kraken_icon_view_can_zoom_out;
	kraken_view_class->clear = kraken_icon_view_clear;
	kraken_view_class->end_loading = kraken_icon_view_end_loading;
	kraken_view_class->file_changed = kraken_icon_view_file_changed;
	kraken_view_class->get_selected_icon_locations = kraken_icon_view_get_selected_icon_locations;
    kraken_view_class->get_selection = kraken_icon_view_get_selection;
    kraken_view_class->peek_selection = kraken_icon_view_peek_selection;
	kraken_view_class->get_selection_count = kraken_icon_view_get_selection_count;
	kraken_view_class->get_selection_for_file_transfer = kraken_icon_view_get_selection;
	kraken_view_class->get_item_count = kraken_icon_view_get_item_count;
	kraken_view_class->is_empty = kraken_icon_view_is_empty;
	kraken_view_class->remove_file = kraken_icon_view_remove_file;
	kraken_view_class->reset_to_defaults = kraken_icon_view_reset_to_defaults;
	kraken_view_class->restore_default_zoom_level = kraken_icon_view_restore_default_zoom_level;
    kraken_view_class->get_default_zoom_level = kraken_icon_view_get_default_zoom_level;
	kraken_view_class->reveal_selection = kraken_icon_view_reveal_selection;
	kraken_view_class->select_all = kraken_icon_view_select_all;
	kraken_view_class->set_selection = kraken_icon_view_set_selection;
	kraken_view_class->invert_selection = kraken_icon_view_invert_selection;
	kraken_view_class->compare_files = compare_files;
	kraken_view_class->zoom_to_level = kraken_icon_view_zoom_to_level;
	kraken_view_class->get_zoom_level = kraken_icon_view_get_zoom_level;
        kraken_view_class->click_policy_changed = kraken_icon_view_click_policy_changed;
        kraken_view_class->click_to_rename_mode_changed = kraken_icon_view_click_to_rename_mode_changed;
        kraken_view_class->merge_menus = kraken_icon_view_merge_menus;
        kraken_view_class->unmerge_menus = kraken_icon_view_unmerge_menus;
        kraken_view_class->sort_directories_first_changed = kraken_icon_view_sort_directories_first_changed;
        kraken_view_class->sort_favorites_first_changed = kraken_icon_view_sort_favorites_first_changed;
        kraken_view_class->start_renaming_file = kraken_icon_view_start_renaming_file;
        kraken_view_class->update_menus = kraken_icon_view_update_menus;
	kraken_view_class->using_manual_layout = kraken_icon_view_using_manual_layout;
	kraken_view_class->widget_to_file_operation_position = kraken_icon_view_widget_to_file_operation_position;
	kraken_view_class->get_view_id = kraken_icon_view_get_id;
	kraken_view_class->get_first_visible_file = icon_view_get_first_visible_file;
	kraken_view_class->scroll_to_file = icon_view_scroll_to_file;

	properties[PROP_COMPACT] =
		g_param_spec_boolean ("compact",
				      "Compact",
				      "Whether this view provides a compact listing",
				      FALSE,
				      G_PARAM_WRITABLE);
	properties[PROP_SUPPORTS_AUTO_LAYOUT] =
		g_param_spec_boolean ("supports-auto-layout",
				      "Supports auto layout",
				      "Whether this view supports auto layout",
				      TRUE,
				      G_PARAM_WRITABLE |
				      G_PARAM_CONSTRUCT_ONLY);
	properties[PROP_IS_DESKTOP] =
		g_param_spec_boolean ("is-desktop",
				      "Is a desktop view",
				      "Whether this view is on a desktop",
				      FALSE,
				      G_PARAM_WRITABLE |
				      G_PARAM_CONSTRUCT_ONLY);
	properties[PROP_SUPPORTS_KEEP_ALIGNED] =
		g_param_spec_boolean ("supports-keep-aligned",
				      "Supports keep aligned",
				      "Whether this view supports keep aligned",
				      FALSE,
				      G_PARAM_WRITABLE |
				      G_PARAM_CONSTRUCT_ONLY);
	properties[PROP_SUPPORTS_LABELS_BESIDE_ICONS] =
		g_param_spec_boolean ("supports-labels-beside-icons",
				      "Supports labels beside icons",
				      "Whether this view supports labels beside icons",
				      TRUE,
				      G_PARAM_WRITABLE |
				      G_PARAM_CONSTRUCT_ONLY);

	g_object_class_install_properties (oclass, NUM_PROPERTIES, properties);
}

static void
kraken_icon_view_init (KrakenIconView *icon_view)
{
    icon_view->details = g_new0 (KrakenIconViewDetails, 1);
    icon_view->details->sort = &sort_criteria[0];
}

static KrakenView *
kraken_icon_view_create (KrakenWindowSlot *slot)
{
	KrakenIconView *view;

	view = g_object_new (KRAKEN_TYPE_ICON_VIEW,
			     "window-slot", slot,
			     NULL);
#if GTK_CHECK_VERSION (3, 20, 0)
	gtk_style_context_add_class (gtk_widget_get_style_context (GTK_WIDGET(view)), GTK_STYLE_CLASS_VIEW);
#endif

    set_compact_view (view, FALSE);

	return KRAKEN_VIEW (view);
}

static KrakenView *
kraken_compact_view_create (KrakenWindowSlot *slot)
{
	KrakenIconView *view;

	view = g_object_new (KRAKEN_TYPE_ICON_VIEW,
			     "window-slot", slot,
			     NULL);
#if GTK_CHECK_VERSION (3, 20, 0)
	gtk_style_context_add_class (gtk_widget_get_style_context (GTK_WIDGET(view)), GTK_STYLE_CLASS_VIEW);
#endif

    set_compact_view (view, TRUE);

    return KRAKEN_VIEW (view);
}

static gboolean
kraken_icon_view_supports_uri (const char *uri,
			   GFileType file_type,
			   const char *mime_type)
{
	if (file_type == G_FILE_TYPE_DIRECTORY) {
		return TRUE;
	}
	if (g_str_has_prefix (uri, "trash:")) {
		return TRUE;
	}
	if (g_str_has_prefix (uri, EEL_SEARCH_URI)) {
		return TRUE;
	}

	return FALSE;
}

#define TRANSLATE_VIEW_INFO(view_info)					\
	view_info.view_combo_label = _(view_info.view_combo_label);	\
	view_info.view_menu_label_with_mnemonic = _(view_info.view_menu_label_with_mnemonic); \
	view_info.error_label = _(view_info.error_label);		\
	view_info.startup_error_label = _(view_info.startup_error_label); \
	view_info.display_location_label = _(view_info.display_location_label); \


static KrakenViewInfo kraken_icon_view = {
	(char *)KRAKEN_ICON_VIEW_ID,
	/* translators: this is used in the view selection dropdown
	 * of navigation windows and in the preferences dialog */
	(char *)N_("Icon View"),
	/* translators: this is used in the view menu */
	(char *)N_("_Icons"),
	(char *)N_("The icon view encountered an error."),
	(char *)N_("The icon view encountered an error while starting up."),
	(char *)N_("Display this location with the icon view."),
	kraken_icon_view_create,
	kraken_icon_view_supports_uri
};

static KrakenViewInfo kraken_compact_view = {
	(char *)FM_COMPACT_VIEW_ID,
	/* translators: this is used in the view selection dropdown
	 * of navigation windows and in the preferences dialog */
	(char *)N_("Compact View"),
	/* translators: this is used in the view menu */
	(char *)N_("_Compact"),
	(char *)N_("The compact view encountered an error."),
	(char *)N_("The compact view encountered an error while starting up."),
	(char *)N_("Display this location with the compact view."),
	kraken_compact_view_create,
	kraken_icon_view_supports_uri
};

gboolean
kraken_icon_view_is_compact (KrakenIconView *view)
{
	return view->details->compact;
}

void
kraken_icon_view_register (void)
{
	TRANSLATE_VIEW_INFO (kraken_icon_view)
		kraken_view_factory_register (&kraken_icon_view);
}

void
kraken_icon_view_compact_register (void)
{
	TRANSLATE_VIEW_INFO (kraken_compact_view)
		kraken_view_factory_register (&kraken_compact_view);
}

