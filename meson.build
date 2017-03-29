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

nolang_pure = executable('nolang_pure',
    sources: [
        'src/pure.cpp',
        'src/parser.cpp',
        'src/compiler.cpp',
        'src/methodcall.cpp',
        'src/puremethod.cpp',
        '3pp/mpc/mpc.c'
    ],
    include_directories: [
        top_inc
    ]
)
