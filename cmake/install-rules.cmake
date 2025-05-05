if(PROJECT_IS_TOP_LEVEL)
  set(
      CMAKE_INSTALL_INCLUDEDIR "include/kettle-${PROJECT_VERSION}"
      CACHE STRING ""
  )
  set_property(CACHE CMAKE_INSTALL_INCLUDEDIR PROPERTY TYPE PATH)
endif()

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# find_package(<package>) call for consumers to find this project
set(package kettle)

install(
    DIRECTORY
    include/
    "${PROJECT_BINARY_DIR}/export/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT kettle_Development
)

install(
    TARGETS kettle_kettle
    EXPORT kettleTargets
    RUNTIME #
    COMPONENT kettle_Runtime
    LIBRARY #
    COMPONENT kettle_Runtime
    NAMELINK_COMPONENT kettle_Development
    ARCHIVE #
    COMPONENT kettle_Development
    INCLUDES #
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

write_basic_package_version_file(
    "${package}ConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion
)

# Allow package maintainers to freely override the path for the configs
set(
    KETTLE_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/${package}"
    CACHE STRING "CMake package config location relative to the install prefix"
)
set_property(CACHE KETTLE_INSTALL_CMAKEDIR PROPERTY TYPE PATH)
mark_as_advanced(KETTLE_INSTALL_CMAKEDIR)

install(
    FILES cmake/install-config.cmake
    DESTINATION "${KETTLE_INSTALL_CMAKEDIR}"
    RENAME "${package}Config.cmake"
    COMPONENT kettle_Development
)

install(
    FILES "${PROJECT_BINARY_DIR}/${package}ConfigVersion.cmake"
    DESTINATION "${KETTLE_INSTALL_CMAKEDIR}"
    COMPONENT kettle_Development
)

install(
    EXPORT kettleTargets
    NAMESPACE kettle::
    DESTINATION "${KETTLE_INSTALL_CMAKEDIR}"
    COMPONENT kettle_Development
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
