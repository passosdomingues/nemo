
kraken_i18n_header = [
  'kraken-extension-i18n.h'
]

kraken_extension_sources = [
  gresources,
  'kraken-column-provider.c',
  'kraken-column.c',
  'kraken-desktop-preferences.c',
  'kraken-extension-private.h',
  'kraken-file-info.c',
  'kraken-info-provider.c',
  'kraken-location-widget-provider.c',
  'kraken-menu-item.c',
  'kraken-menu-provider.c',
  'kraken-name-and-desc-provider.c',
  'kraken-property-page-provider.c',
  'kraken-property-page.c',
  'kraken-menu.c',
  'kraken-simple-button.c',
]

kraken_extension_headers = [
  'kraken-column-provider.h',
  'kraken-column.h',
  'kraken-desktop-preferences.h',
  'kraken-extension-types.h',
  'kraken-file-info.h',
  'kraken-info-provider.h',
  'kraken-location-widget-provider.h',
  'kraken-menu-item.h',
  'kraken-menu-provider.h',
  'kraken-name-and-desc-provider.h',
  'kraken-property-page-provider.h',
  'kraken-property-page.h',
  'kraken-menu.h',
  'kraken-simple-button.h',
]

kraken_extension_incdir = include_directories('.')

kraken_extension_deps = [ glib, gtk ]

kraken_extension_lib = shared_library('kraken-extension',
  kraken_extension_sources + kraken_i18n_header,
  dependencies: kraken_extension_deps,
  include_directories: [ rootInclude, ],
  install: true,
  version: '1.4.0',
  link_args: [ '-Wl,-no-undefined' ],
)

install_headers(kraken_extension_headers,
  subdir: join_paths('kraken', 'libkraken-extension')
)

kraken_extension = declare_dependency(
  include_directories: include_directories('.'),
  link_with: [ kraken_extension_lib ],
  dependencies: kraken_extension_deps,
)

typelibdir = go_intr.get_variable(pkgconfig: 'typelibdir',
  pkgconfig_define: ['libdir', join_paths(get_option('prefix'), get_option('libdir'))]
)

gnome.generate_gir(kraken_extension_lib,
  sources: kraken_extension_sources + kraken_extension_headers,
  nsversion: '3.0',
  namespace: 'Kraken',
  export_packages: 'libkraken-extension',
  includes: [ 'Gtk-3.0', 'Gio-2.0', 'GLib-2.0', ],
  include_directories: [ rootInclude, ],
  install_dir_typelib: typelibdir,
  install: true,
)

pkgconfig.generate(filebase: 'libkraken-extension',
  name: 'libkraken-extension',
  description: 'A library to create Kraken view extensions',
  version: meson.project_version(),
  requires: [ 'gio-2.0', 'glib-2.0', 'gtk+-3.0' ],
  libraries: kraken_extension_lib,
  subdirs: 'kraken',
  variables: 'extensiondir=${libdir}/@0@/@1@'.format('kraken', 'extensions-3.0'),
)

