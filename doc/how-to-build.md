# How To Build

This document explains how to build and run the examples and tests.

## Prerequisites

You need to install a C++14 compiler (for example [gcc][gcc] or [clang][clang]),
[cmake][cmake] and [conan][conan].

[gcc]: https://gcc.gnu.org/
[clang]: http://clang.llvm.org/
[cmake]: https://cmake.org/
[conan]: https://www.conan.io/


## Install Dependencies

You need to compile dependencies properly. Otherwise, you may get errors
(like [this][undefined reference error]):

[undefined reference error]: http://stackoverflow.com/q/41408216/1065654

```
cd .conan
conan install -s build_type=Release -s compiler=clang -s compiler.version=3.8 -s compiler.libcxx=libstdc++11 --build=missing
```

## Build

Create build directory (if it does not exist) and change to it:
```
mkdir cmake-build-release
cd cmake-build-release
```

### Linux or Mac

```sh
cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

### Windows

```sh
cmake .. -G "Visual Studio 14 Win64"
cmake --build . --config Release
```


### Note

You might need to tell *cmake* which compiler you are using:

```
export CC=clang
export CXX=clang++
```

If you use [Clion][Clion] and don't start it from
the shell you have to configure those environment variables in Clion (see
[How to add environment variables in Clion][environment variables in Clion]).
Delete Clion's build directory (e.g. `cmake-build-release`) after you changed
environment variables and run *Tools > CMake > Reload CMake Project*.

[Clion]: https://www.jetbrains.com/clion/
[environment variables in Clion]: http://stackoverflow.com/a/38874446/1065654



## Run

Binaries of examples and tests are created in the build directory `./cmake-build-release/bin/`.

### Tests

Run all tests

```sh
./cmake-build-release/bin/catch
```
