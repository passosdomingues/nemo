dbusBuiltSources = gnome.gdbus_codegen(
  'kraken-freedesktop-generated',
  join_paths(meson.project_source_root(), 'data', 'freedesktop-dbus-interfaces.xml'),
  interface_prefix: 'org.freedesktop.',
  namespace: 'KrakenFreedesktop',
  object_manager: true
)

krakenBuiltSources = gnome.gdbus_codegen(
  'kraken-cinnamon-dbus',
  join_paths(meson.project_source_root(), 'data', 'org.Cinnamon.xml'),
  interface_prefix: 'org.',
  namespace: 'Kraken',
  object_manager: true
)

krakenCommon_sources = [
  dbusBuiltSources,
  krakenBuiltSources,
  'kraken-action-config-widget.c',
  'kraken-application.c',
  'kraken-blank-desktop-window.c',
  'kraken-bookmark-list.c',
  'kraken-bookmarks-window.c',
  'kraken-config-base-widget.c',
  'kraken-connect-server-dialog-nonmain.c',
  'kraken-connect-server-dialog.c',
  'kraken-connect-server-operation.c',
  'kraken-desktop-icon-grid-view.c',
  'kraken-desktop-item-properties.c',
  'kraken-desktop-manager.c',
  'kraken-desktop-overlay.c',
  'kraken-desktop-window.c',
  'kraken-error-reporting.c',
  'kraken-extension-config-widget.c',
  'kraken-file-management-properties.c',
  'kraken-floating-bar.c',
  'kraken-freedesktop-dbus.c',
  'kraken-icon-view-container.c',
  'kraken-icon-view-grid-container.c',
  'kraken-icon-view.c',
  'kraken-image-properties-page.c',
  'kraken-interesting-folder-bar.c',
  'kraken-list-model.c',
  'kraken-list-view.c',
  'kraken-location-bar.c',
  'kraken-location-entry.c',
  'kraken-mime-actions.c',
  'kraken-navigation-action.c',
  'kraken-navigation-state.c',
  'kraken-notebook.c',
  'kraken-pathbar.c',
  'kraken-places-sidebar.c',
  'kraken-plugin-manager.c',
  'kraken-previewer.c',
  'kraken-progress-info-widget.c',
  'kraken-progress-ui-handler.c',
  'kraken-properties-window.c',
  'kraken-query-editor.c',
  'kraken-template-config-widget.c',
  'kraken-self-check-functions.c',
  'kraken-statusbar.c',
  'kraken-thumbnail-problem-bar.c',
  'kraken-toolbar.c',
  'kraken-trash-bar.c',
  'kraken-tree-sidebar-model.c',
  'kraken-tree-sidebar.c',
  'kraken-view-dnd.c',
  'kraken-view-factory.c',
  'kraken-view.c',
  'kraken-graph-view.c',
  'kraken-timeline-view.c',
  'kraken-window-bookmarks.c',
  'kraken-window-manage-views.c',
  'kraken-window-menus.c',
  'kraken-window-pane.c',
  'kraken-window-slot-dnd.c',
  'kraken-window-slot.c',
  'kraken-window.c',
  'kraken-x-content-bar.c',
]

krakenWindow_sources = [
  'kraken-main-application.c',
  'kraken-main.c',
]
krakenWindow_headers = [
  'kraken-main-application.h',
]

krakenDesktop_sources = [
  'kraken-desktop-application.c',
  'kraken-desktop-icon-view.c',
  'kraken-desktop-main.c',
]
krakenDesktop_headers = [
  'kraken-desktop-application.h',
  'kraken-desktop-overlay.h',
  'kraken-desktop-icon-view.h',
]


if enableEmptyView
  krakenCommon_sources += 'kraken-empty-view.c'
endif

kraken_deps = [ cinnamon, gail, glib, gtk, math,
  egg, kraken_extension, kraken_private, xapp ]

if exempi_enabled
  kraken_deps += exempi
endif

if libexif_enabled
  kraken_deps += libexif
endif

kraken = executable('kraken',
  krakenCommon_sources + krakenWindow_sources,
  include_directories: [ rootInclude ],
  c_args: kraken_definitions,
  dependencies: kraken_deps,
  install: true
)

krakenDesktop = executable('kraken-desktop',
  krakenCommon_sources + krakenDesktop_sources,
  include_directories: [ rootInclude],
  c_args: kraken_definitions,
  dependencies: kraken_deps,
  install: true
)

kraken_autorun_software = executable('kraken-autorun-software',
  [ 'kraken-autorun-software.c' ],
  include_directories: [ rootInclude, ],
  c_args: kraken_definitions,
  dependencies: kraken_deps,
  install: true
)

kraken_connect_server = executable('kraken-connect-server',
  [
    'kraken-bookmark-list.c',
    'kraken-connect-server-dialog.c',
    'kraken-connect-server-dialog-main.c',
    'kraken-connect-server-operation.c'
  ],
  include_directories: [ rootInclude, ],
  c_args: kraken_definitions,
  dependencies: kraken_deps,
  install: true
)

kraken_open_with = executable('kraken-open-with',
  [ 'kraken-open-with-main.c' ],
  include_directories: [ rootInclude, ],
  c_args: kraken_definitions,
  dependencies: kraken_deps,
  install: true
)

# LibExecs
kraken_extensions_list = executable('kraken-extensions-list',
  [ 'kraken-extensions-list.c' ],
  include_directories: [ rootInclude, ],
  c_args: kraken_definitions,
  dependencies: [ kraken_deps ],
  install: true,
  install_dir: libExecPath,
)
