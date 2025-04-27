cmake_minimum_required(VERSION 3.14)

project(kettleTests LANGUAGES CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)
include(../cmake/windows-set-path.cmake)

# ---- Dependencies ----

if(PROJECT_IS_TOP_LEVEL)
  find_package(kettle REQUIRED)
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

cmake_path(GET PROJECT_SOURCE_DIR PARENT_PATH KETTLE_SOURCE_DIR)
set(SOURCE_FILES_DIR "${KETTLE_SOURCE_DIR}/source")

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
        PRIVATE kettle::kettle
        PRIVATE Catch2::Catch2WithMain
    )

    catch_discover_tests(${add_test_target_TARGET})
endfunction()

add_test_target(TARGET probabilities_test SOURCES "source/calculations/probabilities_test.cpp")

add_test_target(TARGET circuit_test SOURCES "source/circuit/circuit_test.cpp")

add_test_target(TARGET append_circuits_test SOURCES "source/circuit_operations/append_circuits_test.cpp")
add_test_target(TARGET compare_circuits_test SOURCES "source/circuit_operations/compare_circuits_test.cpp")
add_test_target(TARGET make_binary_controlled_circuit_test SOURCES "source/circuit_operations/make_binary_controlled_circuit_test.cpp")
add_test_target(TARGET make_controlled_circuit_test SOURCES "source/circuit_operations/make_controlled_circuit_test.cpp")
add_test_target(TARGET transpile_to_primitive_test SOURCES "source/circuit_operations/transpile_to_primitive_test.cpp")

add_test_target(TARGET mathtools_test SOURCES "source/common/mathtools_test.cpp")
add_test_target(TARGET linear_bijective_map_test SOURCES "source/common/linear_bijective_map_test.cpp")
add_test_target(TARGET matrix2x2_test SOURCES "source/common/matrix2x2_test.cpp")
add_test_target(TARGET arange_test SOURCES "source/common/arange_test.cpp")

add_test_target(TARGET control_swap_test SOURCES "source/gates/control_swap_test.cpp")
add_test_target(TARGET fourier_test SOURCES "source/gates/fourier_test.cpp")
add_test_target(TARGET gate_test SOURCES "source/gates/gate_test.cpp")
add_test_target(TARGET matrix2x2_gate_decomposition_test SOURCES "source/gates/matrix2x2_gate_decomposition_test.cpp")
add_test_target(TARGET multiplicity_controlled_u_gate_test SOURCES "source/gates/multiplicity_controlled_u_gate_test.cpp")
add_test_target(TARGET random_u_gates_test SOURCES "source/gates/random_u_gates_test.cpp")
add_test_target(TARGET toffoli_test SOURCES "source/gates/toffoli_test.cpp")

add_test_target(TARGET io_control_flow_test SOURCES "source/io/io_control_flow_test.cpp")
add_test_target(TARGET io_numpy_statevector_test SOURCES "source/io/numpy_statevector_test.cpp")
add_test_target(TARGET io_statevector_test SOURCES "source/io/statevector_test.cpp")
add_test_target(TARGET read_tangelo_file_test SOURCES "source/io/read_tangelo_file_test.cpp")
add_test_target(TARGET write_tangelo_file_test SOURCES "source/io/write_tangelo_file_test.cpp")

add_test_target(TARGET control_flow_test SOURCES "source/simulation/control_flow_test.cpp")
add_test_target(TARGET gate_pair_generator_test SOURCES "source/simulation/gate_pair_generator_test.cpp")
add_test_target(TARGET measure_test SOURCES "source/simulation/measure_test.cpp")
add_test_target(TARGET multithread_simulate_utils_test SOURCES "source/simulation/multithread_simulate_utils_test.cpp")
add_test_target(TARGET operations_test SOURCES "source/simulation/operations_test.cpp")
add_test_target(TARGET simulate_test SOURCES "source/simulation/simulate_test.cpp")

add_test_target(TARGET state_test SOURCES "source/state/state_test.cpp")
add_test_target(TARGET bitstring_utils_test SOURCES "source/state/bitstring_utils_test.cpp")

# ---- End-of-file commands ----

add_folders(Test)
