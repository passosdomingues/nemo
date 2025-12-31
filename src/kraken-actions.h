/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/*
 *  Kraken
 *
 *  Copyright (C) 2004 Red Hat, Inc.
 *
 *  Kraken is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  Kraken is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public
 *  License along with this program; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin Street, Suite 500, MA 02110-1335, USA.
 *
 *  Authors: Alexander Larsson <alexl@redhat.com>
 *
 */

#ifndef KRAKEN_ACTIONS_H
#define KRAKEN_ACTIONS_H

#include <glib.h>

#define KRAKEN_ACTION_STOP "Stop"
#define KRAKEN_ACTION_RELOAD "Reload"
#define KRAKEN_ACTION_BACK "Back"
#define KRAKEN_ACTION_COMPUTER "Computer"
#define KRAKEN_ACTION_UP "Up"
#define KRAKEN_ACTION_UP_ACCEL "UpAccel"
#define KRAKEN_ACTION_UP_ACCEL "UpAccel"
#define KRAKEN_ACTION_FORWARD "Forward"
#define KRAKEN_ACTION_SHOW_HIDE_SIDEBAR "Show Hide Sidebar"

#define KRAKEN_ACTION_SHOW_HIDE_TOOLBAR "Show Hide Toolbar"
#define KRAKEN_ACTION_TOOLBAR_ALWAYS_SHOW_PATHBAR "Toolbar Always Show Pathbar"
#define KRAKEN_ACTION_TOOLBAR_ALWAYS_SHOW_ENTRY "Toolbar Always Show Entry"

#define KRAKEN_ACTION_SHOW_HIDE_STATUSBAR "Show Hide Statusbar"
#define KRAKEN_ACTION_SHOW_HIDE_MENUBAR "Show Hide Menubar"
#define KRAKEN_ACTION_SHOW_HIDE_LOCATION_BAR "Show Hide Location Bar"
#define KRAKEN_ACTION_SHOW_HIDE_EXTRA_PANE "Show Hide Extra Pane"
#define KRAKEN_ACTION_GO_TO_BURN_CD "Go to Burn CD"
#define KRAKEN_ACTION_EDIT_LOCATION "Edit Location"
#define KRAKEN_ACTION_COMPACT_VIEW "CompactView"
#define KRAKEN_ACTION_ICON_VIEW "IconView"
#define KRAKEN_ACTION_LIST_VIEW "ListView"
#define KRAKEN_ACTION_GO_HOME "Home"
#define KRAKEN_ACTION_ADD_BOOKMARK "Add Bookmark"
#define KRAKEN_ACTION_EDIT_BOOKMARKS "Edit Bookmarks"
#define KRAKEN_ACTION_HOME "Home"
#define KRAKEN_ACTION_ZOOM_IN "Zoom In"
#define KRAKEN_ACTION_ZOOM_OUT "Zoom Out"
#define KRAKEN_ACTION_ZOOM_NORMAL "Zoom Normal"
#define KRAKEN_ACTION_SHOW_HIDDEN_FILES "Show Hidden Files"
#define KRAKEN_ACTION_CLOSE "Close"
#define KRAKEN_ACTION_SEARCH "Search"
#define KRAKEN_ACTION_FOLDER_WINDOW "Folder Window"
#define KRAKEN_ACTION_NEW_TAB "New Tab"

#define KRAKEN_ACTION_OPEN "Open"
#define KRAKEN_ACTION_OPEN_ALTERNATE "OpenAlternate"
#define KRAKEN_ACTION_OPEN_IN_NEW_TAB "OpenInNewTab"
#define KRAKEN_ACTION_LOCATION_OPEN_ALTERNATE "LocationOpenAlternate"
#define KRAKEN_ACTION_LOCATION_OPEN_IN_NEW_TAB "LocationOpenInNewTab"
#define KRAKEN_ACTION_OTHER_APPLICATION1 "OtherApplication1"
#define KRAKEN_ACTION_OTHER_APPLICATION2 "OtherApplication2"
#define KRAKEN_ACTION_NEW_FOLDER "New Folder"
#define KRAKEN_ACTION_PROPERTIES "Properties"
#define KRAKEN_ACTION_PROPERTIES_ACCEL "PropertiesAccel"
#define KRAKEN_ACTION_LOCATION_PROPERTIES "LocationProperties"
#define KRAKEN_ACTION_NO_TEMPLATES "No Templates"
#define KRAKEN_ACTION_EMPTY_TRASH "Empty Trash"
#define KRAKEN_ACTION_CUT "Cut"
#define KRAKEN_ACTION_LOCATION_CUT "LocationCut"
#define KRAKEN_ACTION_COPY "Copy"
#define KRAKEN_ACTION_LOCATION_COPY "LocationCopy"
#define KRAKEN_ACTION_PASTE "Paste"
#define KRAKEN_ACTION_PASTE_FILES_INTO "Paste Files Into"
#define KRAKEN_ACTION_COPY_TO_NEXT_PANE "Copy to next pane"
#define KRAKEN_ACTION_MOVE_TO_NEXT_PANE "Move to next pane"
#define KRAKEN_ACTION_COPY_TO_HOME "Copy to Home"
#define KRAKEN_ACTION_MOVE_TO_HOME "Move to Home"
#define KRAKEN_ACTION_COPY_TO_DESKTOP "Copy to Desktop"
#define KRAKEN_ACTION_MOVE_TO_DESKTOP "Move to Desktop"
#define KRAKEN_ACTION_BROWSE_MOVE_TO "BrowseMoveTo"
#define KRAKEN_ACTION_BROWSE_COPY_TO "BrowseCopyTo"
#define KRAKEN_ACTION_COPY_TO_MENU "CopyToMenu"
#define KRAKEN_ACTION_MOVE_TO_MENU "MoveToMenu"
#define KRAKEN_ACTION_LOCATION_PASTE_FILES_INTO "LocationPasteFilesInto"
#define KRAKEN_ACTION_RENAME "Rename"
#define KRAKEN_ACTION_DUPLICATE "Duplicate"
#define KRAKEN_ACTION_CREATE_LINK "Create Link"
#define KRAKEN_ACTION_SELECT_ALL "Select All"
#define KRAKEN_ACTION_INVERT_SELECTION "Invert Selection"
#define KRAKEN_ACTION_SELECT_PATTERN "Select Pattern"
#define KRAKEN_ACTION_TRASH "Trash"
#define KRAKEN_ACTION_LOCATION_TRASH "LocationTrash"
#define KRAKEN_ACTION_DELETE "Delete"
#define KRAKEN_ACTION_LOCATION_DELETE "LocationDelete"
#define KRAKEN_ACTION_RESTORE_FROM_TRASH "Restore From Trash"
#define KRAKEN_ACTION_LOCATION_RESTORE_FROM_TRASH "LocationRestoreFromTrash"
#define KRAKEN_ACTION_CONNECT_TO_SERVER_LINK "Connect To Server Link"
#define KRAKEN_ACTION_MOUNT_VOLUME "Mount Volume"
#define KRAKEN_ACTION_UNMOUNT_VOLUME "Unmount Volume"
#define KRAKEN_ACTION_EJECT_VOLUME "Eject Volume"
#define KRAKEN_ACTION_START_VOLUME "Start Volume"
#define KRAKEN_ACTION_STOP_VOLUME "Stop Volume"
#define KRAKEN_ACTION_POLL "Poll"
#define KRAKEN_ACTION_SELF_MOUNT_VOLUME "Self Mount Volume"
#define KRAKEN_ACTION_SELF_UNMOUNT_VOLUME "Self Unmount Volume"
#define KRAKEN_ACTION_SELF_EJECT_VOLUME "Self Eject Volume"
#define KRAKEN_ACTION_SELF_START_VOLUME "Self Start Volume"
#define KRAKEN_ACTION_SELF_STOP_VOLUME "Self Stop Volume"
#define KRAKEN_ACTION_SELF_POLL "Self Poll"
#define KRAKEN_ACTION_LOCATION_MOUNT_VOLUME "Location Mount Volume"
#define KRAKEN_ACTION_LOCATION_UNMOUNT_VOLUME "Location Unmount Volume"
#define KRAKEN_ACTION_LOCATION_EJECT_VOLUME "Location Eject Volume"
#define KRAKEN_ACTION_LOCATION_START_VOLUME "Location Start Volume"
#define KRAKEN_ACTION_LOCATION_STOP_VOLUME "Location Stop Volume"
#define KRAKEN_ACTION_LOCATION_POLL "Location Poll"
#define KRAKEN_ACTION_SCRIPTS "Scripts"
#define KRAKEN_ACTION_ACTIONS "Actions"
#define KRAKEN_ACTION_NEW_DOCUMENTS "New Documents"
#define KRAKEN_ACTION_NEW_EMPTY_DOCUMENT "New Empty Document"
#define KRAKEN_ACTION_EMPTY_TRASH_CONDITIONAL "Empty Trash Conditional"
#define KRAKEN_ACTION_MANUAL_LAYOUT "Manual Layout"
#define KRAKEN_ACTION_REVERSED_ORDER "Reversed Order"
#define KRAKEN_ACTION_CLEAN_UP "Clean Up"
#define KRAKEN_ACTION_KEEP_ALIGNED "Keep Aligned"
#define KRAKEN_ACTION_ARRANGE_ITEMS "Arrange Items"
#define KRAKEN_ACTION_STRETCH "Stretch"
#define KRAKEN_ACTION_UNSTRETCH "Unstretch"
#define KRAKEN_ACTION_ZOOM_ITEMS "Zoom Items"
#define KRAKEN_ACTION_SORT_TRASH_TIME "Sort by Trash Time"
#define KRAKEN_ACTION_OPEN_AS_ROOT "OpenAsRoot"
#define KRAKEN_ACTION_TOGGLE_LOCATION "Toggle Location Button"

#define KRAKEN_ACTION_STATUSBAR_PLACES "Statusbar Places"
#define KRAKEN_ACTION_STATUSBAR_TREEVIEW "Statusbar Treeview"
#define KRAKEN_ACTION_STATUSBAR_SIDEBAR_TOGGLE "Statusbar Sidebar Toggle"

#define KRAKEN_ACTION_OPEN_IN_TERMINAL "OpenInTerminal"
#define KRAKEN_ACTION_FOLLOW_SYMLINK "FollowSymbolicLink"
#define KRAKEN_ACTION_OPEN_CONTAINING_FOLDER "OpenContainingFolder"

#define KRAKEN_ACTION_PLUGIN_MANAGER "KrakenPluginManager"

#define KRAKEN_ACTION_SHOW_THUMBNAILS "Show Thumbnails"
#define KRAKEN_ACTION_SHOW_FULL_CONTEXT_MENU "ShowFullContextMenu"

#define KRAKEN_ACTION_PIN_FILE        "Pin File"
#define KRAKEN_ACTION_UNPIN_FILE      "Unpin File"
#define KRAKEN_ACTION_FAVORITE_FILE        "Favorite File"
#define KRAKEN_ACTION_UNFAVORITE_FILE      "Unfavorite File"
#define KRAKEN_ACTION_DESKTOP_OVERLAY "Desktop Overlay"

#define KRAKEN_ACTION_SIDEBAR_REMOVE "Remove Bookmark"
#define KRAKEN_ACTION_SIDEBAR_DETECT_MEDIA "Detect Media"

typedef struct
{
    const gchar  *action_name; // The action's name
    const gchar  *config_widget_name; // The builder id of the corresponding toggle in preferences
                                      // Can be null if the action's should be tied to another
                                      // action's visibility.
    const gchar  *ui_path; // The xml path of the item from the ui file
    const gchar  *settings_key;; // The gsettings key corresponding to the menu item's visibility.
} ConfigurableMenuItemInfo;

static const ConfigurableMenuItemInfo CONFIGURABLE_MENU_ITEM_INFO [] = {
    // Selection
    { KRAKEN_ACTION_OPEN, "selection_menu__open_check",
     "/selection/Open Placeholder/Open", "selection-menu-open" },

    { KRAKEN_ACTION_OPEN_IN_NEW_TAB, "selection_menu__open_in_new_tab_check",
     "/selection/Open Placeholder/OpenInNewTab", "selection-menu-open-in-new-tab" },

    { KRAKEN_ACTION_OPEN_ALTERNATE, "selection_menu__open_in_new_window_check",
     "/selection/Open Placeholder/OpenAlternate", "selection-menu-open-in-new-window" },

    { KRAKEN_ACTION_SCRIPTS, "selection_menu__scripts_check",
     "/selection/Open Placeholder/Scripts", "selection-menu-scripts" },

    { KRAKEN_ACTION_CUT, "selection_menu__cut_check",
     "/selection/File Clipboard Actions/Cut", "selection-menu-cut" },

    { KRAKEN_ACTION_COPY, "selection_menu__copy_check",
     "/selection/File Clipboard Actions/Copy", "selection-menu-copy" },

    { KRAKEN_ACTION_PASTE_FILES_INTO, "selection_menu__paste_check",
     "/selection/File Clipboard Actions/Paste Files Into", "selection-menu-paste" },

    { KRAKEN_ACTION_DUPLICATE, "selection_menu__duplicate_check",
     "/selection/File Clipboard Actions/Duplicate", "selection-menu-duplicate" },

    { KRAKEN_ACTION_PIN_FILE, "selection_menu__pin_check",
     "/selection/File Actions/Pin File", "selection-menu-pin" },
    { KRAKEN_ACTION_UNPIN_FILE, NULL,
     "/selection/File Actions/Unpin File", "selection-menu-pin" },

    { KRAKEN_ACTION_FAVORITE_FILE, "selection_menu__favorite_check",
     "/selection/File Actions/Favorite File", "selection-menu-favorite" },
    { KRAKEN_ACTION_UNFAVORITE_FILE, NULL,
     "/selection/File Actions/Unfavorite File", "selection-menu-favorite" },

    { KRAKEN_ACTION_CREATE_LINK, "selection_menu__make_link_check",
     "/selection/File Actions/Create Link", "selection-menu-make-link" },

    { KRAKEN_ACTION_RENAME, "selection_menu__rename_check",
     "/selection/File Actions/Rename", "selection-menu-rename" },

    { KRAKEN_ACTION_COPY_TO_MENU, "selection_menu__copy_to_check",
     "/selection/File Actions/CopyToMenu", "selection-menu-copy-to" },

    { KRAKEN_ACTION_MOVE_TO_MENU, "selection_menu__move_to_check",
     "/selection/File Actions/MoveToMenu", "selection-menu-move-to" },

    { KRAKEN_ACTION_OPEN_IN_TERMINAL, "selection_menu__open_in_terminal_check",
     "/selection/OpenInTerminal", "selection-menu-open-in-terminal" },

    { KRAKEN_ACTION_OPEN_AS_ROOT, "selection_menu__open_as_root_check",
     "/selection/OpenAsRoot", "selection-menu-open-as-root" },

    { KRAKEN_ACTION_TRASH, "selection_menu__move_to_trash_check",
     "/selection/Dangerous File Actions/Trash", "selection-menu-move-to-trash" },

    { KRAKEN_ACTION_PROPERTIES, "selection_menu__properties_check",
     "/selection/Properties", "selection-menu-properties" },

     // Background

    { KRAKEN_ACTION_NEW_FOLDER, "background_menu__create_new_folder_check",
     "/background/Before Zoom Items/New Object Items/New Folder", "background-menu-create-new-folder" },

    { KRAKEN_ACTION_SCRIPTS, "background_menu__scripts_check",
     "/background/Before Zoom Items/New Object Items/Scripts", "background-menu-scripts" },

    { KRAKEN_ACTION_OPEN_IN_TERMINAL, "background_menu__open_in_terminal_check",
     "/background/Before Zoom Items/OpenInTerminal", "background-menu-open-in-terminal" },

    { KRAKEN_ACTION_OPEN_AS_ROOT, "background_menu__open_as_root_check",
     "/background/Before Zoom Items/OpenAsRoot", "background-menu-open-as-root" },

    { KRAKEN_ACTION_SHOW_HIDDEN_FILES, "background_menu__show_hidden_files_check",
     "/background/Before Zoom Items/Show Hidden Files", "background-menu-show-hidden-files" },

    { KRAKEN_ACTION_PASTE, "background_menu__paste_check",
     "/background/Before Zoom Items/File Clipboard Actions/Paste", "background-menu-paste" },

    { KRAKEN_ACTION_PROPERTIES, "background_menu__properties_check",
     "/background/Folder Items Placeholder/Properties", "background-menu-properties" },

     // Icon View (merged with background)
    { KRAKEN_ACTION_ARRANGE_ITEMS, "iconview_menu__arrange_items_check",
     "/background/Before Zoom Items/View Items/Arrange Items", "iconview-menu-arrange-items" },

    { KRAKEN_ACTION_CLEAN_UP, "iconview_menu__organize_by_name_check",
     "/background/Before Zoom Items/View Items/Clean Up", "iconview-menu-organize-by-name" },

     // Desktop (new)
    { KRAKEN_ACTION_DESKTOP_OVERLAY, "desktop_menu__customize_check",
     "/background/Before Zoom Items/View Items/Desktop Overlay", "desktop-menu-customize" },
};

#define CONFIGURABLE_MENU_ITEM_COUNT (G_N_ELEMENTS (CONFIGURABLE_MENU_ITEM_INFO))

#endif /* KRAKEN_ACTIONS_H */
