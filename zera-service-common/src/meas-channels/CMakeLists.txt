set(HEADER
    justnode.h
    )

add_library(zerameaschannels SHARED
    ${HEADER}
    justnode.cpp
    )

target_link_libraries(zerameaschannels
    PUBLIC
    Qt5::Core
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

# announce headers - target perspective
target_include_directories(zerameaschannels
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/zerameaschannels>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

set_target_properties(zerameaschannels PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zerameaschannels PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(zerameaschannels)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zerameaschannels_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/zerameaschannels
    )

install(TARGETS zerameaschannels
    EXPORT zerameaschannelsExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

# configure *ConfigVersion.cmake
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    zerameaschannelsConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMajorVersion
    )

# configure *Config.cmake
configure_file(zerameaschannelsConfig.cmake.in zerameaschannelsConfig.cmake @ONLY)

# install *Config(Version).cmake
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/zerameaschannelsConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/zerameaschannelsConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zerameaschannels
    )

# install targets cmake-files
install(EXPORT zerameaschannelsExport
    FILE zerameaschannelsTargets.cmake
    NAMESPACE ZeraClasses::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zerameaschannels
    )

# install public headers
install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/zerameaschannels
    )
