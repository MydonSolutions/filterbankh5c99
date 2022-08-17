test_file_stempath = './simon.h5'

cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required : true)

test('simon', executable(
  'simon', ['simon.c'],
	  dependencies: [lib_filterbankh5c99_dep, m_dep],
  ),
  args: [],
  timeout: false,
  priority : -1
)
