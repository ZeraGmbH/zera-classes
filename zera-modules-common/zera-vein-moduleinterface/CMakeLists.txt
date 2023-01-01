file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(zera-vein-moduleinterface SHARED
    ${SOURCES}
    ${HEADER}
    )

target_link_libraries(zera-vein-moduleinterface
    PUBLIC
    VeinMeta::VfEvent
    VeinMeta::VfComponent
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(zera-vein-moduleinterface
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-vein-moduleinterface>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

set_target_properties(zera-vein-moduleinterface PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zera-vein-moduleinterface PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(zera-vein-moduleinterface)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zera-vein-moduleinterface_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-vein-moduleinterface
    )

install(TARGETS zera-vein-moduleinterface
    EXPORT zera-vein-moduleinterfaceExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

write_basic_package_version_file(
    zera-vein-moduleinterfaceConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMajorVersion
    )

configure_file(zera-vein-moduleinterfaceConfig.cmake.in zera-vein-moduleinterfaceConfig.cmake @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/zera-vein-moduleinterfaceConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/zera-vein-moduleinterfaceConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zera-vein-moduleinterface
    )

install(EXPORT zera-vein-moduleinterfaceExport
    FILE zera-vein-moduleinterfaceTargets.cmake
    NAMESPACE ZeraClasses::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zera-vein-moduleinterface
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-vein-moduleinterface
    )
