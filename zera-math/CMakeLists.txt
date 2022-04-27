file(GLOB SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(zeramath SHARED
    ${SOURCES}
    ${HEADER}
    )

target_link_libraries(zeramath 
    PRIVATE
    Qt5::Core
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

# announce headers - target perspective
target_include_directories(zeramath
    PUBLIC
    $<INSTALL_INTERFACE:include/zera-classes/math>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

set_target_properties(zeramath PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zeramath PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

include(GenerateExportHeader)
generate_export_header(zeramath)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zeramath_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/math
    )

install(TARGETS zeramath
    EXPORT zeramathExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

# configure *ConfigVersion.cmake
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    zeramathConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMinorVersion
    )

# configure *Config.cmake
configure_file(zeramathConfig.cmake.in zeramathConfig.cmake @ONLY)

# install *Config(Version).cmake
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/zeramathConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/zeramathConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zeramath
    )

# install targets cmake-files
install(EXPORT zeramathExport
    FILE zeramathTargets.cmake
    NAMESPACE ZeraClasses::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zeramath
    )

# install public headers
install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/math
    )
