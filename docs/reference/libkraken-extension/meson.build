
docConf = configuration_data()
docConf.set('KRAKEN_EXTENSION_VERSION_INFO', KRAKEN_EXTENSION_VERSION_INFO)

configure_file(
  input : 'version.xml.in',
  output: 'version.xml',
  configuration: docConf
)

gnome.gtkdoc('libkraken-extension',
  main_xml: 'libkraken-extension-docs.xml',
  src_dir: join_paths(meson.project_source_root(), 'libkraken-extension'),
  dependencies: kraken_extension,
  install: true,
  gobject_typesfile: 'libkraken-extension.types',
  install_dir: 'libkraken-extension',
  scan_args: [
    '--rebuild-types',
    '--ignore-headers="config.h"' ]
)
