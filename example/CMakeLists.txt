cmake_minimum_required(VERSION 3.14)

project(kettleExamples CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)

if(PROJECT_IS_TOP_LEVEL)
  find_package(kettle REQUIRED)
endif()

add_custom_target(run-examples)

function(add_example SUBDIR NAME)
    add_executable("${NAME}" "${SUBDIR}/${NAME}.cpp")
    target_link_libraries("${NAME}" PRIVATE kettle::kettle)
    target_compile_features("${NAME}" PRIVATE cxx_std_20)
    add_custom_target("run_${NAME}" COMMAND "${NAME}" VERBATIM)
    add_dependencies("run_${NAME}" "${NAME}")
    add_dependencies(run-examples "run_${NAME}")
endfunction()

function(add_example_with_nlopt SUBDIR NAME)
    add_executable("${NAME}" "${SUBDIR}/${NAME}.cpp")
    target_link_libraries("${NAME}" PRIVATE kettle::kettle)
    target_compile_features("${NAME}" PRIVATE cxx_std_20)

    get_target_property(NLOPT_INCLUDES nlopt::nlopt INTERFACE_INCLUDE_DIRECTORIES)
    target_link_libraries("${NAME}" PRIVATE nlopt::nlopt)
    target_include_directories("${NAME}" SYSTEM PUBLIC ${NLOPT_INCLUDES})

    add_custom_target("run_${NAME}" COMMAND "${NAME}" VERBATIM)
    add_dependencies("run_${NAME}" "${NAME}")
    add_dependencies(run-examples "run_${NAME}")
endfunction()

add_example("algorithms" deutsch_jozsa_balanced_ibm_example)
add_example("algorithms" deutsch_jozsa)
add_example("algorithms" deutsch)
add_example("algorithms" grover_two_qubit)
add_example("algorithms" quantum_phase_estimation_t_gate)
add_example("algorithms" quantum_teleportation)
add_example("algorithms" shor)
add_example("algorithms" simon_ibm_example)
add_example_with_nlopt("algorithms" vqe_basic)

add_example("general" circuit_with_control_flow)
add_example("general" creating_the_bell_state)
add_example("general" inverse_fourier)
add_example("general" random_state)
add_example("general" save_statevector_example)

add_folders(Example)
