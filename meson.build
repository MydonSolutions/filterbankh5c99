project('filterbankh5c99', 'c',
  version: '1.1.0',
  default_options: [
    'c_std=c99',
    'buildtype=release',
    'b_ndebug=if-release'
  ]
)

src_lst = []
inc_lst = [
	include_directories('include'),
]
dep_lst = [
  dependency('hdf5'),
  subproject('filterbankc99').get_variable('lib_filterbankc99_dep')
]

subdir('include')
subdir('src')

lib_filterbankh5c99 = library(
  'filterbankh5c99',
  src_lst,
  include_directories: inc_lst,
  dependencies: dep_lst,
  install: true,
)

lib_filterbankh5c99_dep = declare_dependency(
  include_directories: inc_lst,
  dependencies: dep_lst,
  link_with: lib_filterbankh5c99,
)

pkg = import('pkgconfig')
pkg.generate(
  lib_filterbankh5c99,
  libraries: dep_lst,
)

subdir('tests')