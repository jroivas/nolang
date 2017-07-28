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
        'src/namespacedef.cpp',
        'src/parsers/argumentparser.cpp',
        'src/parsers/assignmentparser.cpp',
        'src/parsers/constparser.cpp',
        'src/parsers/importparser.cpp',
        'src/parsers/namespacedefparser.cpp',
        'src/parsers/numberparser.cpp',
        'src/parsers/methodparser.cpp',
        'src/parsers/methodcallparser.cpp',
        'src/parsers/structparser.cpp',
        'src/parsers/typeidentparser.cpp',
        'src/codegen/cgen.cpp',
        'src/codegen/typesolver.cpp',
        'src/codegen/nativetypesolver.cpp',
        'src/codegen/nolangtypesolver.cpp',
        'src/codegen/methodcallgenerator.cpp',
        'src/codegen/blockgenerator.cpp',
        '3pp/mpc/mpc.c'
    ],
    cpp_args : ['-std=c++11'],
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
    cpp_args : ['-std=c++11'],
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
    cpp_args : ['-std=c++11'],
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
    cpp_args : ['-std=c++11'],
    link_with: [
        nolang_lib
    ]
)

test_assignmentparser = executable('test-assignmentparser',
    sources: [
        'test/test_assignmentparser.cpp'
    ],
    include_directories: [
        top_inc,
        src_inc,
    ],
    cpp_args : ['-std=c++11'],
    link_with: [
        nolang_lib
    ]
)

test('test_parser', test_parser)
test('test_compiler', test_compiler)
test('test_assignmentparser', test_assignmentparser)

checks = find_program('checks.sh')
test('test_cppcheck', checks)
