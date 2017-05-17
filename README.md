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


## License

MIT, see more [LICENSE](LICENSE)
