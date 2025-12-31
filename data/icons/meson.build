publicIcons = [
  'hicolor/apps/16x16/kraken.png',
  'hicolor/apps/22x22/kraken.png',
  'hicolor/apps/24x24/kraken.png',
  'hicolor/apps/32x32/kraken.png',
  'hicolor/apps/scalable/kraken.svg',
  'hicolor/actions/16x16/kraken-eject.png',
  'hicolor/actions/16x16/menu-bullet.png',
  'hicolor/actions/16x16/menu-sort-up.png',
  'hicolor/actions/16x16/menu-sort-up-free.png',
  'hicolor/actions/16x16/menu-sort-down.png',
  'hicolor/actions/16x16/menu-sort-down-free.png',
  'hicolor/actions/16x16/menu-sort-right.png',
  'hicolor/actions/16x16/menu-sort-right-free.png',
  'hicolor/actions/16x16/menu-sort-left.png',
  'hicolor/actions/16x16/menu-sort-left-free.png',
  'hicolor/actions/16x16/menu-none.png',
  'hicolor/actions/32x32/kraken-eject.png',
  'hicolor/actions/scalable/kraken-location-symbolic.svg',
  'hicolor/actions/scalable/kraken-location-symbolic-rtl.svg',
  'hicolor/actions/scalable/kraken-sidebar-hide-symbolic.svg',
  'hicolor/actions/scalable/kraken-sidebar-hide-symbolic-rtl.svg',
  'hicolor/actions/scalable/kraken-sidebar-show-symbolic.svg',
  'hicolor/actions/scalable/kraken-sidebar-show-symbolic-rtl.svg',
  'hicolor/actions/scalable/kraken-sidebar-tree-symbolic.svg',
  'hicolor/actions/scalable/kraken-sidebar-tree-symbolic-rtl.svg',
  'hicolor/actions/scalable/kraken-sidebar-places-symbolic.svg',
  'hicolor/actions/scalable/kraken-auto-arrange-symbolic.svg',
  'hicolor/actions/scalable/kraken-desktop-scale-symbolic.svg',
  'hicolor/actions/scalable/kraken-horizontal-layout-symbolic.svg',
  'hicolor/actions/scalable/kraken-horizontal-layout-wide-symbolic.svg',
  'hicolor/actions/scalable/kraken-vertical-layout-symbolic.svg',
  'hicolor/actions/scalable/kraken-vertical-layout-wide-symbolic.svg',
  'hicolor/actions/scalable/kraken-recursive-search-symbolic.svg',
  'hicolor/actions/scalable/kraken-recursive-search-symbolic-rtl.svg',
  'hicolor/status/scalable/kraken-progress-0-symbolic.svg',
  'hicolor/status/scalable/kraken-progress-10-symbolic.svg',
  'hicolor/status/scalable/kraken-progress-20-symbolic.svg',
  'hicolor/status/scalable/kraken-progress-30-symbolic.svg',
  'hicolor/status/scalable/kraken-progress-40-symbolic.svg',
  'hicolor/status/scalable/kraken-progress-50-symbolic.svg',
  'hicolor/status/scalable/kraken-progress-60-symbolic.svg',
  'hicolor/status/scalable/kraken-progress-70-symbolic.svg',
  'hicolor/status/scalable/kraken-progress-80-symbolic.svg',
  'hicolor/status/scalable/kraken-progress-90-symbolic.svg',
  'hicolor/status/scalable/kraken-progress-100-symbolic.svg',
]
privateIcons = [
  'hicolor/emblems/16x16/emblem-note.png',
  'hicolor/emblems/24x24/emblem-note.png',
  'hicolor/emblems/48x48/emblem-note.png',
]
noInstallIcons = [
  'hicolor/apps/16x16/kraken.svg',
  'hicolor/apps/22x22/kraken.svg',
  'hicolor/apps/32x32/kraken.svg',
]

foreach icon : publicIcons
  nameParts = icon.split('/')
  theme   = nameParts[0]
  context = nameParts[1]
  size    = nameParts[2]
  file    = nameParts[3]
  install_data(icon,
    install_dir: join_paths(get_option('datadir'), 'icons', theme, size, context)
  )
endforeach


foreach icon : privateIcons
  nameParts = icon.split('/')
  theme   = nameParts[0]
  context = nameParts[1]
  size    = nameParts[2]
  file    = nameParts[3]
  install_data(icon,
    install_dir: join_paths(get_option('datadir'), 'kraken', 'icons', theme, size, context)
  )
endforeach

