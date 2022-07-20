file(GLOB SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(zeraxmlconfig SHARED
    ${SOURCES}
    ${HEADER}
    )

target_link_libraries(zeraxmlconfig 
    PRIVATE
    Qt5::Core
    Qt5::XmlPatterns
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

# announce headers - target perspective
target_include_directories(zeraxmlconfig
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zeraxmlconfig>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

set_target_properties(zeraxmlconfig PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zeraxmlconfig PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(zeraxmlconfig)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zeraxmlconfig_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zeraxmlconfig
    )

install(TARGETS zeraxmlconfig
    EXPORT zeraxmlconfigExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

# configure *ConfigVersion.cmake
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    zeraxmlconfigConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMajorVersion
    )

# configure *Config.cmake
configure_file(zeraxmlconfigConfig.cmake.in zeraxmlconfigConfig.cmake @ONLY)

# install *Config(Version).cmake
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/zeraxmlconfigConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/zeraxmlconfigConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zeraxmlconfig
    )

# install targets cmake-files
install(EXPORT zeraxmlconfigExport
    FILE zeraxmlconfigTargets.cmake
    NAMESPACE ZeraClasses::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zeraxmlconfig
    )

# install public headers
install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zeraxmlconfig
    )

