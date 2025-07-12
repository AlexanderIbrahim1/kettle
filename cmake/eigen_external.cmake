# set(BUILD_TESTING OFF CACHE INTERNAL "") 
# 
# include(FetchContent)
# FetchContent_Declare(
#   eigen
#   GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
#   GIT_TAG        3.4.0
#   GIT_SHALLOW TRUE
# )
# 
# # set(CMAKE_POLICY_DEFAULT_CMP0077 NEW) 
# 
# FetchContent_GetProperties(eigen)
# if(NOT eigen_POPULATED)
#     FetchContent_Populate(eigen)
#     # add_subdirectory(${eigen_SOURCE_DIR} ${eigen_BINARY_DIR})
#     add_library(Eigen3::Eigen INTERFACE IMPORTED GLOBAL)
#     set_target_properties(Eigen3::Eigen PROPERTIES
#         INTERFACE_INCLUDE_DIRECTORIES "${eigen_SOURCE_DIR}"
#     )
# endif()
# 
# if(NOT TARGET Eigen3::Eigen)
#   add_library(Eigen3::Eigen ALIAS eigen)
# endif()
# 
# set(BUILD_TESTING ON CACHE INTERNAL "") 

# if (TARGET Eigen3::Eigen)
#     return()
# endif()
# 
# include(FetchContent)
# FetchContent_Declare(
#     eigen
#     GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
#     GIT_TAG tags/3.4.0
#     GIT_SHALLOW TRUE
# )
# FetchContent_GetProperties(eigen)
# if(NOT eigen_POPULATED)
#     FetchContent_Populate(eigen)
# endif()
# 
# add_library(Eigen3_Eigen INTERFACE)
# add_library(Eigen3::Eigen ALIAS Eigen3_Eigen)
# 
# target_include_directories(Eigen3_Eigen SYSTEM INTERFACE
#     $<BUILD_INTERFACE:${eigen_SOURCE_DIR}>
# )

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW) 

set(BUILD_TESTING OFF)

include(FetchContent)
FetchContent_Declare(
    Eigen
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG master
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)
# note: To disable eigen tests,
# you should put this code in a add_subdirectory to avoid to change
# BUILD_TESTING for your own project too since variables are directory
# scoped
set(EIGEN_BUILD_TESTING OFF)
set(EIGEN_MPL2_ONLY ON)
set(EIGEN_BUILD_PKGCONFIG OFF)
set(EIGEN_BUILD_DOC OFF)
set(EIGEN_BUILD_DOC_DEFAULT OFF)
FetchContent_MakeAvailable(Eigen)
# FetchContent_GetProperties(eigen)
# if(NOT eigen_POPULATED)
#     FetchContent_Populate(eigen)
# endif()

set(BUILD_TESTING ON)

# add_library(Eigen3::Eigen ALIAS Eigen)