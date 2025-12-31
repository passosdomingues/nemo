kraken_daemon_deps = [
  glib,
  gio,
  json,
  dependency('sqlite3'),
  math,
  kraken_lib_dep,
]

libkraken_core = static_library('kraken-core',
  'kraken-daemon-impl.c',
  dependencies: kraken_daemon_deps,
  include_directories: [rootInclude, include_directories('../kraken-lib')]
)

kraken_core_dep = declare_dependency(
  link_with: libkraken_core,
  include_directories: [include_directories('.')]
)

executable('kraken-daemon',
  'kraken-daemon.c',
  dependencies: [kraken_core_dep, kraken_daemon_deps],
  install: true,
  install_dir: get_option('prefix') / get_option('bindir')
)

executable('kraken-search-test',
  'kraken-search-test.c',
  dependencies: [kraken_core_dep, kraken_daemon_deps],
  install: false
)
