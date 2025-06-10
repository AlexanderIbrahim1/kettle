if (TARGET Eigen3::Eigen)
    return()
endif()

include(FetchContent)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
FetchContent_Declare(
    Eigen3
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG 3.4.0
    GIT_SHALLOW TRUE
    SOURCE_SUBDIR cmake
)
FetchContent_MakeAvailable(Eigen3)

add_library(Eigen3::Eigen INTERFACE IMPORTED)
set_target_properties(Eigen3::Eigen PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${eigen3_SOURCE_DIR})
set(EIGEN3_INCLUDE_DIR ${eigen3_SOURCE_DIR})
