file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )


file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(zera-intel-hex SHARED
    ${SOURCES}
    ${HEADER}
    )

target_link_libraries(zera-intel-hex
    PUBLIC
    Qt5::Core
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(zera-intel-hex
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/zera-intel-hex>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

set_target_properties(zera-intel-hex PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zera-intel-hex PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(zera-intel-hex)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zera-intel-hex_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/zera-intel-hex
    )

install(TARGETS zera-intel-hex
    EXPORT zera-intel-hexExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

write_basic_package_version_file(
    zera-intel-hexConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMajorVersion
    )

configure_file(zera-intel-hexConfig.cmake.in zera-intel-hexConfig.cmake @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/zera-intel-hexConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/zera-intel-hexConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zera-intel-hex
    )

install(EXPORT zera-intel-hexExport
    FILE zera-intel-hexTargets.cmake
    NAMESPACE ZeraClasses::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zera-intel-hex
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/zera-intel-hex
    )
