
dbusBuiltSources = gnome.gdbus_codegen('kraken-generated',
  join_paths(meson.project_source_root(), 'data', 'dbus-interfaces.xml'),
  interface_prefix: 'org.Kraken.',
  namespace: 'KrakenDBus',
  object_manager: true
)

kraken_private_sources = [
  dbusBuiltSources,
  'kraken-action-manager.c',
  'kraken-action.c',
  'kraken-bookmark.c',
  'kraken-cell-renderer-disk.c',
  'kraken-centered-placement-grid.c',
  'kraken-clipboard-monitor.c',
  'kraken-clipboard.c',
  'kraken-column-chooser.c',
  'kraken-column-utilities.c',
  'kraken-dbus-manager.c',
  'kraken-debug.c',
  'kraken-default-file-icon.c',
  'kraken-desktop-directory-file.c',
  'kraken-desktop-directory.c',
  'kraken-desktop-icon-file.c',
  'kraken-desktop-link-monitor.c',
  'kraken-desktop-link.c',
  'kraken-desktop-metadata.c',
  'kraken-desktop-utils.c',
  'kraken-directory-async.c',
  'kraken-directory.c',
  'kraken-dnd.c',
  'kraken-entry.c',
  'kraken-file-changes-queue.c',
  'kraken-file-conflict-dialog.c',
  'kraken-file-dnd.c',
  'kraken-file-operations.c',
  'kraken-file-queue.c',
  'kraken-file-undo-manager.c',
  'kraken-file-undo-operations.c',
  'kraken-file-utilities.c',
  'kraken-file.c',
  'kraken-global-preferences.c',
  'kraken-icon-canvas-item.c',
  'kraken-icon-container.c',
  'kraken-icon-dnd.c',
  'kraken-icon-info.c',
  'kraken-job-queue.c',
  'kraken-lib-self-check-functions.c',
  'kraken-link.c',
  'kraken-merged-directory.c',
  'kraken-metadata.c',
  'kraken-mime-application-chooser.c',
  'kraken-module.c',
  'kraken-monitor.c',
  'kraken-placement-grid.c',
  'kraken-places-tree-view.c',
  'kraken-program-choosing.c',
  'kraken-progress-info-manager.c',
  'kraken-progress-info.c',
  'kraken-query.c',
  'kraken-recent.c',
  'kraken-search-directory-file.c',
  'kraken-search-directory.c',
  'kraken-search-engine-advanced.c',
  'kraken-search-engine-kraken.c',
  'kraken-search-engine.c',
  'kraken-selection-canvas-item.c',
  'kraken-separator-action.c',
  'kraken-signaller.c',
  'kraken-thumbnails.c',
  'kraken-trash-monitor.c',
  'kraken-tree-view-drag-dest.c',
  'kraken-ui-utilities.c',
  'kraken-undo-manager.c',
  'kraken-undo-signal-handlers.c',
  'kraken-undo-transaction.c',
  'kraken-undo.c',
  'kraken-vfs-directory.c',
  'kraken-vfs-file.c',
  'kraken-widget-action.c',
  'kraken-widget-menu-item.c',
  'kraken-tag-manager.c',
  'kraken-tag-directory.c',
  'kraken-tag-file.c',
]

kraken_private_deps = [
  cinnamon, eel, gail, gio_unix, glib, gmodule, gtk, json, math, kraken_extension, x11, xapp, kraken_core_dep
]

if libexif_enabled
  kraken_private_deps += libexif
endif

if tracker_enabled
  kraken_private_sources += 'kraken-search-engine-tracker.c'
  kraken_private_deps += tracker_sparql
endif

if libselinux_enabled
  kraken_private_deps += libselinux
endif

kraken_private_lib = static_library('kraken-private',
  kraken_private_sources,
  dependencies: kraken_private_deps,
  include_directories: [ rootInclude, ],
  c_args: kraken_definitions,
)

kraken_private = declare_dependency(
  include_directories: include_directories('.'),
  link_with: [ kraken_private_lib ],
  dependencies: kraken_private_deps,
)

install_data('org.kraken.gschema.xml',
    install_dir: join_paths(get_option('datadir'), 'glib-2.0', 'schemas')
)
