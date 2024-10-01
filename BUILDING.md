# Building with CMake

## Build

Here are the steps for users to build in high performance mode with the
Unix Makefiles generator:
```sh
cmake --preset=highperf
cmake --build --preset=highperf
```

To build in ordinary release mode:
```sh
cmake -S . -B build -D CMAKE_BUILD_TYPE=Release
cmake --build build
```

To build in debug mode:
```sh
cmake -S . -B build -D CMAKE_BUILD_TYPE=Debug
cmake --build build
```

Here are the steps for building in release mode with a multi-configuration
generator, like the Visual Studio ones:
```sh
cmake -S . -B build
cmake --build build --config Release
```

### Building with MSVC

Note that MSVC by default is not standards compliant and you need to pass some
flags to make it behave properly. See the `flags-windows` preset in the
[CMakePresets.json](CMakePresets.json) file for the flags and with what
variable to provide them to CMake during configuration.

### Building on Apple Silicon

CMake supports building on Apple Silicon properly since 3.20.1. Make sure you
have the [latest version][1] installed.

[1]: https://cmake.org/download/
