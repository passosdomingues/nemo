libgsf = dependency('libgsf-1', required: true)

mso_to_txt_sources = [
  'kraken-mso-to-txt.c'
]

mso_to_txt = executable('kraken-mso-to-txt',
  mso_to_txt_sources,
  dependencies: [libgsf, gio, glib],
  install: true
)

ppt_to_txt_sources = [
  'kraken-ppt-to-txt.c'
]

ppt_to_txt = executable('kraken-ppt-to-txt',
  ppt_to_txt_sources,
  dependencies: [gio, glib],
  install: true
)

install_data(
    ['kraken-xls-to-txt', 'kraken-odf-to-txt'],
    install_dir: join_paths(get_option('prefix'), get_option('bindir')),
    install_mode: 'rwxr-xr-x'
)

install_data(
    'epub2text.kraken_search_helper',
    'mso.kraken_search_helper',
    'mso-ppt.kraken_search_helper',
    'mso-xls.kraken_search_helper',
    'odf.kraken_search_helper',
    install_dir: join_paths(krakenDataPath, 'search-helpers')
)

install_data(
    'kraken-epub2text',
    install_dir: get_option('bindir')
)

subdir('third-party')