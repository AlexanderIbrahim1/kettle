if(TARGET nlopt::nlopt)
    return()
endif()

set(AUTODIFF_BUILD_TESTS OFF CACHE BOOL "Enable the compilation of the test files." FORCE)
set(AUTODIFF_BUILD_PYTHON OFF CACHE BOOL "Enable the compilation of the python bindings." FORCE)
set(AUTODIFF_BUILD_EXAMPLES OFF CACHE BOOL "Enable the compilation of the example files." FORCE)
set(AUTODIFF_BUILD_DOCS OFF CACHE BOOL "Enable the build of the documentation and website." FORCE)

set(NLOPT_CXX OFF CACHE BOOL "enable cxx routines" FORCE)
set(NLOPT_FORTRAN OFF CACHE BOOL "enable fortran" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build NLopt as a shared library" FORCE)
set(NLOPT_PYTHON OFF CACHE BOOL "build python bindings" FORCE)
set(NLOPT_OCTAVE OFF CACHE BOOL "build octave bindings" FORCE)
set(NLOPT_MATLAB OFF CACHE BOOL "build matlab bindings" FORCE)
set(NLOPT_GUILE OFF CACHE BOOL "build guile bindings" FORCE)
set(NLOPT_SWIG OFF CACHE BOOL "use SWIG to build bindings" FORCE)

# the CMakeLists.txt file for nlopt still tries to use 'Python_VERSION_MINOR' and 'Python_VERSION_MAJOR',
# even though setting 'NLOPT_PYTHON' means that the 'find_package(Python)' call is never performed;
# so we need to find it anyways
find_package(Python)

# these two are also not defined?
set(CPACK_SOURCE_IGNORE_FILES ".git;/build;.*~;")
set(NLOPT_DEFINITIONS "")

include(FetchContent)
FetchContent_Declare(
    nlopt
    URL https://github.com/stevengj/nlopt/archive/refs/tags/v2.10.0.zip
    EXCLUDE_FROM_ALL
    DOWNLOAD_EXTRACT_TIMESTAMP ON
    FIND_PACKAGE_ARGS NAMES nlopt
)

FetchContent_MakeAvailable(nlopt)
add_library(nlopt::nlopt ALIAS nlopt)
