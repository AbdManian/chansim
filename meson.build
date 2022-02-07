project('chansim', 'cpp')

sclib = dependency('systemc', version : '>=2.3')

srcs = ['src/main.cpp']

cs_exe = executable('chansim', srcs, dependencies : [sclib])


test('runmain', cs_exe)