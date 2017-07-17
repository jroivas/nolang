# Nolang

Nolang is experimental programming language.

It bases on idea of "pure" programming, where one has no side effects
and variables are by default immutable.

Please read [pure](pure.md) for more info.


## Design rules

- No classes (no OOP)
- No GC
- Automatic memory management, similar to Rust
- Develop friendly
- Strong types
- Immutable by default
- No side effects

## Reference compiler

This repository contains reference compiler and it's implementation.

General parser has grammar defined in BNF.
Compiler generates proper AST, which can be passed to next step.
"Next step" here means code generator or interpreter.

Currently only supported and targeted code generator is plain C backend.
Thus code is transpiled to C code, and then compiled with any C compiler to native binary.

Other code generator backends may be possible in future.

## Building

Get [Meson](http://mesonbuild.com/) and [Ninja](https://ninja-build.org/).

Clone git repository, cd into it and then:

    mkdir build
    cd build
    meson ..
    ninja

To compile single example and run:

    ../compile.sh ../examples/hello.nolang
    ./hello.out

To run test suites:

    ninja test

To build and run all examples do (currently majority not passing):

    ../test.sh

## Extra dependencies

For development:

 - cppcheck
    * http://cppcheck.sourceforge.net/
    * sudo apt-get install cppcheck
    * run as `ninja test` or `./checks.sh`

## License

MIT, see more [LICENSE](LICENSE)
