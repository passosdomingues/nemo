kraken_extension_sources = [
  'kraken-sidebar-provider.c'
]

kraken_extension_deps = [
  glib,
  gtk,
  dependency('sqlite3'),
  kraken_extension # Internal dependency to libkraken-extension
]

shared_module('kraken-sidebar-extension',
  kraken_extension_sources,
  dependencies: kraken_extension_deps,
  include_directories: [ rootInclude ],
  install: true,
  install_dir: krakenExtensionPath
)
