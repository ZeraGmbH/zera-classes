file(GLOB SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(zeradev SHARED
    ${SOURCES}
    ${HEADER}
    )

target_link_libraries(zeradev
    PUBLIC
    Qt5::Core
    zerai2c
    zeramath
    zeramisc
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

# announce headers - target perspective
target_include_directories(zeradev
    PUBLIC
    $<INSTALL_INTERFACE:include/zera-classes/dev>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

set_target_properties(zeradev PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zeradev PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(zeradev)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zeradev_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/dev
    )

install(TARGETS zeradev
    EXPORT zeradevExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

# configure *ConfigVersion.cmake
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    zeradevConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMinorVersion
    )

# configure *Config.cmake
configure_file(zeradevConfig.cmake.in zeradevConfig.cmake @ONLY)

# install *Config(Version).cmake
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/zeradevConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/zeradevConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zeradev
    )

# install targets cmake-files
install(EXPORT zeradevExport
    FILE zeradevTargets.cmake
    NAMESPACE ZeraClasses::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zeradev
    )

# install public headers
install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/dev
    )


