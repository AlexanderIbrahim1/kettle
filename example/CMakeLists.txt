cmake_minimum_required(VERSION 3.14)

project(kettleExamples CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)

if(PROJECT_IS_TOP_LEVEL)
  find_package(kettle REQUIRED)
endif()

add_custom_target(run-examples)

# ---- Get access to multiple argument parser ----
include(CMakeParseArguments)

function(add_example)
    set(OPTIONS USE_NLOPT USE_EIGEN)
    set(ONE_VALUE_ARGS SUBDIR NAME)
    cmake_parse_arguments(add_example "${OPTIONS}" "${ONE_VALUE_ARGS}" "${MULTI_VALUE_ARGS}" "${ARGN}")

    add_executable("${add_example_NAME}" "${add_example_SUBDIR}/${add_example_NAME}.cpp")
    target_link_libraries("${add_example_NAME}" PRIVATE kettle::kettle)
    target_compile_features("${add_example_NAME}" PRIVATE cxx_std_20)

    if(add_example_USE_NLOPT)
        get_target_property(NLOPT_INCLUDES nlopt::nlopt INTERFACE_INCLUDE_DIRECTORIES)
        target_link_libraries("${add_example_NAME}" PRIVATE nlopt::nlopt)
        target_include_directories("${add_example_NAME}" SYSTEM PUBLIC ${NLOPT_INCLUDES})
    endif()

    if(add_example_USE_EIGEN)
        get_target_property(EIGEN_INCLUDES Eigen3::Eigen INTERFACE_INCLUDE_DIRECTORIES)
        target_link_libraries("${add_example_NAME}" PRIVATE Eigen3::Eigen)
        target_include_directories("${add_example_NAME}" SYSTEM PUBLIC ${EIGEN_INCLUDES})
    endif()

    add_custom_target("run_${add_example_NAME}" COMMAND "${add_example_NAME}" VERBATIM)
    add_dependencies("run_${add_example_NAME}" "${add_example_NAME}")
    add_dependencies(run-examples "run_${add_example_NAME}")
endfunction()

add_example(SUBDIR "algorithms" NAME deutsch_jozsa_balanced_ibm_example)
add_example(SUBDIR "algorithms" NAME deutsch_jozsa)
add_example(SUBDIR "algorithms" NAME deutsch)
add_example(SUBDIR "algorithms" NAME grover_two_qubit)
add_example(SUBDIR "algorithms" NAME quantum_phase_estimation_t_gate)
add_example(SUBDIR "algorithms" NAME quantum_teleportation)
add_example(SUBDIR "algorithms" NAME shor)
add_example(SUBDIR "algorithms" NAME simon_ibm_example)
add_example(USE_NLOPT SUBDIR "algorithms" NAME vqe_basic)

add_example(SUBDIR "general" NAME circuit_with_control_flow)
add_example(SUBDIR "general" NAME creating_the_bell_state)
add_example(SUBDIR "general" NAME inverse_fourier)
add_example(SUBDIR "general" NAME random_state)
add_example(SUBDIR "general" NAME save_statevector_example)
add_example(USE_EIGEN SUBDIR "general" NAME hello_eigen)

add_folders(Example)
