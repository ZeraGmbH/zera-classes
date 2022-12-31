file(GLOB SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(zeraproxy SHARED
    ${SOURCES}
    ${HEADER}
    )

target_link_libraries(zeraproxy 
    PUBLIC
    Qt5::Network
    VeinMeta::xiqnet
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(zeraproxy
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/proxy>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

set_target_properties(zeraproxy PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zeraproxy PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(zeraproxy)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zeraproxy_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/proxy
    )

install(TARGETS zeraproxy
    EXPORT zeraproxyExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

write_basic_package_version_file(
    zeraproxyConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMajorVersion
    )

configure_file(zeraproxyConfig.cmake.in zeraproxyConfig.cmake @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/zeraproxyConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/zeraproxyConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zeraproxy
    )

install(EXPORT zeraproxyExport
    FILE zeraproxyTargets.cmake
    NAMESPACE ZeraClasses::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zeraproxy
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/proxy
    )
