cmake_minimum_required(VERSION 3.14)

project(kettleQPETangeloProject CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)

if(PROJECT_IS_TOP_LEVEL)
  find_package(kettle REQUIRED)
endif()

function(add_exec NAME)
    add_executable("${NAME}" "${NAME}.cpp")
    target_link_libraries("${NAME}" PRIVATE kettle::kettle)
    target_compile_features("${NAME}" PRIVATE cxx_std_20)
    add_custom_target("run_${NAME}" COMMAND "${NAME}" VERBATIM)
    add_dependencies("run_${NAME}" "${NAME}")
endfunction()

function(add_exec_with_nlopt NAME)
    add_executable("${NAME}" "${NAME}.cpp")
    target_link_libraries("${NAME}" PRIVATE kettle::kettle PRIVATE nlopt::nlopt)
    target_compile_features("${NAME}" PRIVATE cxx_std_20)

    get_target_property(NLOPT_INCLUDES nlopt::nlopt INTERFACE_INCLUDE_DIRECTORIES)
    target_link_libraries("${NAME}" PRIVATE nlopt::nlopt)
    target_include_directories("${NAME}" SYSTEM PUBLIC ${NLOPT_INCLUDES})

    add_custom_target("run_${NAME}" COMMAND "${NAME}" VERBATIM)
    add_dependencies("run_${NAME}" "${NAME}")
endfunction()

add_exec(qpe_tangelo_eigenvalue)
add_exec(qpe_tangelo_measure)
add_exec(qpe_tangelo_project)
add_exec(qpe_tangelo_simulate)
add_exec(qpe_tangelo_simulate_minimal)
add_exec(qpe_tangelo_simulate_reverse)
add_exec_with_nlopt(vqe_dipolar_linear_rotors)

add_folders(qpe_tangelo_project)
