gresources = gnome.compile_resources(
  'kraken-resources', 'kraken.gresource.xml',
  source_dir: '.',
  c_name: 'kraken',
  install_header: false,
  install: false
)

layout_editor_gresources = gnome.compile_resources(
  'kraken-action-layout-editor-resources', 'kraken-action-layout-editor.gresource.xml',
  source_dir: '.',
  gresource_bundle: true,
  install: true,
  install_dir: krakenDataPath
)
