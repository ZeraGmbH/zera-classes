set(HEADER
    meas-channels/justnode.h
    resources/rmconnection.h
    )

add_library(zeraservercommon SHARED
    ${HEADER}
    meas-channels/justnode.cpp
    resources/rmconnection.cpp
    )

target_link_libraries(zeraservercommon
    PUBLIC
    Qt5::Core
    VeinMeta::xiqnet
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

# announce headers - target perspective
target_include_directories(zeraservercommon
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/zeraservercommon>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

set_target_properties(zeraservercommon PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zeraservercommon PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(zeraservercommon)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zeraservercommon_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/zeraservercommon
    )

install(TARGETS zeraservercommon
    EXPORT zeraservercommonExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

# configure *ConfigVersion.cmake
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    zeraservercommonConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMajorVersion
    )

# configure *Config.cmake
configure_file(zeraservercommonConfig.cmake.in zeraservercommonConfig.cmake @ONLY)

# install *Config(Version).cmake
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/zeraservercommonConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/zeraservercommonConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zeraservercommon
    )

# install targets cmake-files
install(EXPORT zeraservercommonExport
    FILE zeraservercommonTargets.cmake
    NAMESPACE ZeraClasses::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zeraservercommon
    )

# install public headers
install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/zeraservercommon
    )
