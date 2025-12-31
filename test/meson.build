
test('Search Engine test',
  executable('test-kraken-search-engine',
    [ 'test-kraken-search-engine.c' ],
    include_directories: [ rootInclude, ],
    dependencies: [ gtk, kraken_private ],
  ),
  args: []
)

test('Directory Async test',
  executable('test-kraken-directory-async',
    [ 'test-kraken-directory-async.c' ],
    include_directories: [ rootInclude, ],
    dependencies: [ gtk, kraken_private ],
  ),
  args: []
)

test('Copy test',
  executable('test-kraken-copy',
    [ 'test-copy.c', 'test.c' ],
    include_directories: [ rootInclude, ],
    dependencies: [ gtk, kraken_private ],
    c_args: kraken_definitions,
  ),
  args: []
)

executable('test-kraken-uri',
  [ 'test-kraken-uri.c' ],
  include_directories: [ rootInclude, ],
  dependencies: [ gtk, kraken_private ],
  c_args: kraken_definitions,
)
