# Nolang Modules

Modules are most important way to extend Nolang functionality.

Compability with existing libraries is important,
and writing comples wrappers is not meaningful.
Thus Nolang tries to solve this problem with it's module system.


## Module descriptor

Module descriptor is just JSON file, which explains what module does,
what it export and supports.

In perfect world one would just need a library written with any programming languange
and descriptor file, to explain how to map it into Nolang world.

Unfortunately we live in unperfect world, so we need to do little more extra.

## Module introduction

Descriptor starts with introduction:

    {
        'magic': 'nolang_module',
        'descriptor_version': '0.1',
        'module': 'IO',
        'version': '0.1',
        'author': 'Nolang Project',
        'license': 'MIT'
    }

Mandatory fields are:

 - `magic`
 - `descriptor_version`
 - `module`
 - `version`

Other fields are optional, and unknown fields are ignored.

## Module implementation

Descriptor needs to add implementation details.

Module may consist any number of Nolang source files, or prebuilt libraries.

In case of libraries, static or shared linking must be defined.
In case of sources, one can define to build shared library.

Add implementation section, which is list of different subparts of module.
Subparts may be mixed of libraries or sources.

    {
        'magic': 'nolang_module',
        'descriptor_version': '0.1',
        'module': 'IO',
        'version': '0.1',
        'implementation: [
            {
                'type': 'source',
                'sources': [
                    'print.nolang',
                    'read.nolang',
                ],
                'link': 'shared'
            },
            {
                'type': 'library',
                'name': 'native_stdio',
                'link': 'shared'
            },
            {
                'type': 'library',
                'name': 'nolang_adaptations',
                'link': 'static'
            }
        ],
        'includes': [
            'stdio.h'
        ]
    }

On previous example, module consist three different subpart.
First one is Nolang source code, which is compiled when module is imported.
If link type is `static` or missing, module is handled as static part of application.
In case of 'shared', a shared library is built. It's name is autoselected.
On this case it would be something like `libIO_01_1`.

Rest of the file defines prebuilt libraries, which are just linked to the app.
Only options are the type of the linkage, static of shared.

## Exports

On case of code written with some other programming language, we might need mappings.

Also, not all symbols are automatically usable from modules. One needs to define what to export.
Thus we combine these two, to get easily understandable exports.

    {
        'magic': 'nolang_module',
        'descriptor_version': '0.1',
        'module': 'IO',
        'version': '0.1',
        'implementation: [
            {
                'type': 'library',
                'name': 'native_stdio',
                'link': 'shared'
            }
        ],
        exports: [
            {
                'name': 'print',
                'typemap': {
                    'float': 'double'
                },
                'mapping': {
                    'int': { 'name': 'print_int' },
                    'uint': { 'name': 'print_uint' },
                    'double': { 'name': 'print_float' },
                    'string': { 'name': 'print_string' },
                    'list': { 'name': 'print_list' },
                    'void': { 'name': 'println' },
                    '*': { 'name': 'print_any' },
                }
            }
        ]
    }

This demonstrates simple mapping for `IO.print` command.
Depending the type of parameter, separate method is called.
It's possible to define just `int` or `uint` to match any sized integer or unsigned interger.
`typemap` tells how to map certain types. This case all floats are first converted to double values.
Special type `void` means method didn't get any parameters, on that case this implementation is called.
There's also wildcard `*` which means that any other type should be mapped to this method.
Practically `void *` typed pointer is passed in that case.

### Export syntax

Export statement starts with name of the symbol:

    {
        'name': 'exported_symbol_name',
        'typemap': {},
        'mapping': {}
    }

Typemap is simple casting or conversion table,
where a type mapping from type A to type B is defined.
Invalid conversions are not allowed,
so conversion must be possible according Nolang type casting rules.
Example:

    {
        'int8': 'int32',
        'int16': 'int32',
        'uint8': 'int32',
        'uint16': 'int32'
    }

The real mapping is defined under `mapping` key:

    {
        'type1': {
            'name': 'library_symbol_name_for_type1'
        },
        'type2': {
            'name': 'library_symbol_name_for_type2'
        },
        'type3': {
            'name': 'library_symbol_name_for_type3',
            'return': 'return_type',
            'to': 'wrapper_script_to_call_before_passing',
            'from': 'wrapper_script_to_call_before_returning_value',
        }
    }

We could have these C methods:

    uint32_t doubleValue_uint32(uint32_t val) { return val * 2; }
    uint64_t doubleValue_uint64(uint64_t val) { return val * 2; }
    double doubleValue_double(double val) { return val * 2; }
    char *doubleValue_string(const char *val) {
        char *res = malloc(strlen(val)*2+1);
        strcpy(res, val);
        strcpy(res + strlen(val), val);
        return res;
    }

    uint64_t *doubleValue_Date(uint64_t *date) {
        return res * 2;
    }

Let's define simple wrappers:

    Date *date_from_uint(uint64_t val) {
        Date *res = date_new();
        res->millisecondsSinceEpoch = val;
        return res;
    }

    uint64_t date_to_uint(Date *val) {
        return res->millisecondsSinceEpoch;
    }

Mapping these to Nolang is easy:

    {
        'name': 'doubleValue',
        'typemap': {
            'int': 'uint',
            'float': 'double'
        },
        'mapping': {
            'uint': {
                'name': 'doubleValue_uint32',
                'return': 'uint32',
            },
            'uint64': {
                'name': 'doubleValue_uint64',
                'return': 'uint64',
            },
            'double': {
                'name': 'doubleValue_double',
                'return': 'double',
            },
            'string': {
                'name': 'doubleValue_string',
                'return': 'string'
            },
            'Date': {
                'name': 'doubleValue_Date',
                'return': 'Date',
                'to': 'date_to_uint',
                'from': 'date_from_uint'
            }
        }
    }

Thus for example in Nolang these function calls:

    res : String = doubleValue("Hello")
    date : Date = doubleValue(oldDate)

Would become C wise:

    char *res = doubleValue_string("Hello");
    Date *date = date_from_uint(doubleValue_Date(date_to_uint(old_date)));

See how converters are applied on the `Date` case.

### Wrapping pointers

Since C and C++ expose, and work, heavily on pointers,
it might be needed to expose raw pointer to Nolang.
It should be done with structs and `unsafe` type.
This way Nolang compiler knows it should not touch that value.

Let's say we want to build bindings for example [libcurl](https://curl.haxx.se/libcurl/) library to Nolang.
It has it's own objects and structures to store data.


For simple example we need these methods from the library:

 - `CURL *curl_easy_init()`
 - `CURLcode curl_easy_setopt(CURL *, CURLoption, param)`
 - `CURLcode curl_easy_perform(CURL *)`
 - `void curl_easy_cleanup(CURL *)`

Special types:

 - `CURL *`
 - `CURLcode`

Simple type definition written in Nolang, `curl_def.nolang`:

    struct nolang_CURL {
        data : unsafe
    }

This is most probably converted to C code:

    struct nolang_CURL {
        void *data;
    }

Special type `unsafe` is ignored in Nolang code and optimization,
since we have no idea what is it's real type.

Separate wrapper library `curl_wrappers` written in C:

    CURL *to_curl(nolang_CURL* c) {
        if (c == NULL) return NULL;
        return (CURL*)c->data;
    }

    nolang_CURL *from_curl(CURL* c) {
        nolang_CURL *res = (nolang_CURL*)malloc(sizeof(nolang_CURL));
        res->data = c;
        return res;
    }

    uint32_t from_curlcode(CURLcode c) {
        return (uint32_t)c;
    }

    uint32_t curl_seturl_wrapper(CURL*c, char *url) {
        reuturn curl_easy_setopt(c, CURLOPT_URL, ulr);
    }

And module definition:

    {
        'magic': 'nolang_module',
        'descriptor_version': '0.1',
        'module': 'curl',
        'version': '0.1',
        'implementation: [
            {
                'type': 'source',
                'sources': [
                    'curl_def.nolang'
                ],
                'link': 'static'
            },
            {
                'type': 'library',
                'name': 'libcurl',
                'link': 'shared'
            },
            {
                'type': 'library',
                'name': 'curl_wrappers',
                'link': 'shared'
            }
        ],
        'includes': [
            'curl/curl.h',
            'curl_wrappers.h'
        ],
        'exports': [
            {
                'name': 'curl_easy_init',
                'mapping': {
                    'void': {
                        'name': 'curl_easy_init',
                        'return': 'nolang_CURL',
                        'from': 'from_curl'
                    }
                }
            },
            {
                'name': 'curl_seturl',
                'mapping': {
                    'nolang_CURL, string': {
                        'name': 'curl_seturl_wrapper',
                        'return': 'uint32',
                        'to': [
                            'to_curl',
                            '-'
                        ],
                        'from': 'from_curlcode'
                    }
                }
            },
            {
                'name': 'curl_easy_perform',
                'mapping': {
                    'nolang_CURL': {
                        'name': 'curl_easy_perform',
                        'return': 'uint32',
                        'to': 'to_curl',
                        'from': 'from_curlcode'
                    }
                }
            },
            {
                'name': 'curl_easy_cleanup',
                'mapping': {
                    'nolang_CURL': {
                        'name': 'curl_easy_cleanup',
                        'to': 'to_curl'
                    }
                }
            }
        ]
    }

First note is that input parameters can be any amount.
Parameter types are separated with comma, as done with `curl_seturl`.
On that case, `to` mappings needs to be a list, and trivial conversion parameters are marked with dash `-`.
