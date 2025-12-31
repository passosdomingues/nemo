kraken_lib_sources = [
  'toon.c',
  'kraken-llm.c'
]

kraken_lib_deps = [
  glib,
  json,
  dependency('libcurl')
]

kraken_lib = static_library('kraken-lib',
  kraken_lib_sources,
  dependencies: kraken_lib_deps,
  include_directories: include_directories('.')
)

kraken_lib_dep = declare_dependency(
  link_with: kraken_lib,
  include_directories: include_directories('.'),
  dependencies: [glib, json]
)
