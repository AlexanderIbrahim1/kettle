cmake_minimum_required(VERSION 3.14)

project(mini-qiskitTests LANGUAGES CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)
include(../cmake/windows-set-path.cmake)

# ---- Dependencies ----

if(PROJECT_IS_TOP_LEVEL)
  find_package(mini-qiskit REQUIRED)
  enable_testing()
endif()

# ---- Get access to Catch2 ----

include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY git@github.com:catchorg/Catch2.git
    GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(Catch2)
include(Catch)

# ---- Get access to multiple argument parser ----
include(CMakeParseArguments)

# ---- Tests ----

cmake_path(GET PROJECT_SOURCE_DIR PARENT_PATH MINI_QISKIT_SOURCE_DIR)
set(SOURCE_FILES_DIR "${MINI_QISKIT_SOURCE_DIR}/source")

function(add_test_target)
    set(ONE_VALUE_ARGS TARGET)
    set(MULTI_VALUE_ARGS SOURCES)
    cmake_parse_arguments(add_test_target "" "${ONE_VALUE_ARGS}" "${MULTI_VALUE_ARGS}" "${ARGN}")

    add_executable(
        ${add_test_target_TARGET}
        ${add_test_target_SOURCES}
    )

    target_compile_features(
        ${add_test_target_TARGET}
        PRIVATE cxx_std_20
    )

    target_link_libraries(
        ${add_test_target_TARGET}
        PRIVATE mini-qiskit::mini-qiskit
        PRIVATE Catch2::Catch2WithMain
    )

    catch_discover_tests(${add_test_target_TARGET})
endfunction()

# add_executable(mini-qiskit_test source/mini-qiskit_test.cpp)
# target_link_libraries(mini-qiskit_test PRIVATE mini-qiskit::mini-qiskit)
# target_compile_features(mini-qiskit_test PRIVATE cxx_std_20)

# add_test(NAME mini-qiskit_test COMMAND mini-qiskit_test)

add_test_target(TARGET mini-qiskit_test SOURCES "source/mini-qiskit_test.cpp")

# ---- End-of-file commands ----

add_folders(Test)
