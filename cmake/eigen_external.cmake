if (TARGET Eigen3::Eigen)
    return()
endif()

include(FetchContent)
FetchContent_Declare(
    eigen
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG tags/3.4.0
    GIT_SHALLOW TRUE
)
FetchContent_GetProperties(eigen)
if(NOT eigen_POPULATED)
    FetchContent_Populate(eigen)
endif()

add_library(Eigen3_Eigen INTERFACE)
add_library(Eigen3::Eigen ALIAS Eigen3_Eigen)

target_include_directories(Eigen3_Eigen SYSTEM INTERFACE
    $<BUILD_INTERFACE:${eigen_SOURCE_DIR}>
)
