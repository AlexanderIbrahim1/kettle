# Don't want to run the tests that Eigen runs on its own
set(BUILD_TESTING OFF)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW) 

# NOTE: CMake's FetchContent only works for 'master', so I can't fix the version
# - as of the time of actively developing this, the built version is 3.4.90
# LINK TO ISSUE: https://gitlab.com/libeigen/eigen/-/issues/2440
include(FetchContent)
FetchContent_Declare(
    Eigen
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG master
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)

# a bunch of flags that prevent more issues with building Eigen
set(EIGEN_BUILD_TESTING OFF)
set(EIGEN_MPL2_ONLY ON)
set(EIGEN_BUILD_PKGCONFIG OFF)
set(EIGEN_BUILD_DOC OFF)
set(EIGEN_BUILD_DOC_DEFAULT OFF)
FetchContent_MakeAvailable(Eigen)

# We don't want to disable building our own tests
set(BUILD_TESTING ON)