file(GLOB SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(zerai2c SHARED
    ${SOURCES}
    ${HEADER}
    )

target_link_libraries(zerai2c
    PUBLIC
    Qt5::Core
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(zerai2c
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/i2c>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

set_target_properties(zerai2c PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zerai2c PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(zerai2c)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zerai2c_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/i2c
    )

install(TARGETS zerai2c
    EXPORT zerai2cExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

write_basic_package_version_file(
    zerai2cConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMajorVersion
    )

configure_file(zerai2cConfig.cmake.in zerai2cConfig.cmake @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/zerai2cConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/zerai2cConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zerai2c
    )

install(EXPORT zerai2cExport
    FILE zerai2cTargets.cmake
    NAMESPACE ZeraClasses::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zerai2c
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/i2c
    )
