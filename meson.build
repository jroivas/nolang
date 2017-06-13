project('nolang-pure', 'cpp', 'c')

add_global_arguments('-std=c++11', language : 'cpp')
add_global_arguments('-fPIC', language : 'cpp')
add_global_arguments('-Wall', language : 'cpp')
add_global_arguments('-Wno-error=unused-local-typedefs', language : 'cpp')
add_global_arguments('-Wno-unused-local-typedefs', language : 'cpp')
add_global_arguments('-Wno-error=pedantic', language : 'cpp')
add_global_arguments('-DGC_OPERATOR_NEW_ARRAY', language : 'cpp')

add_global_arguments('-std=c99', language : 'c')
add_global_arguments('-fPIC', language : 'c')
add_global_arguments('-Wall', language : 'c')

top_inc = include_directories('.')
src_inc = include_directories('src')

nolang_lib = static_library('nolang',
    [
        'src/tools.cpp',
        'src/pure.cpp',
        'src/parser.cpp',
        'src/compiler.cpp',
        'src/methodcall.cpp',
        'src/puremethod.cpp',
        'src/moduledef.cpp',
        'src/codegen/cgen.cpp',
        'src/codegen/methodcallgenerator.cpp',
        '3pp/mpc/mpc.c'
    ],
    include_directories: [
        top_inc,
        src_inc,
    ]
)

nolang_pure = executable('nolang-pure',
    sources: [
        'src/pure.cpp'
    ],
    include_directories: [
        top_inc,
        src_inc,
    ],
    link_with: [
        nolang_lib
    ]
)

test_parser = executable('test-parser',
    sources: [
        'test/test_parse.cpp'
    ],
    include_directories: [
        top_inc,
        src_inc,
    ],
    link_with: [
        nolang_lib
    ]
)

test_compiler = executable('test-compiler',
    sources: [
        'test/test_compiler.cpp'
    ],
    include_directories: [
        top_inc,
        src_inc,
    ],
    link_with: [
        nolang_lib
    ]
)

test('test_parser', test_parser)
test('test_compiler', test_compiler)
