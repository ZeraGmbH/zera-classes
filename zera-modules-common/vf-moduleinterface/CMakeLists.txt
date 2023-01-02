file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(vf-moduleinterface SHARED
    ${SOURCES}
    ${HEADER}
    )

target_link_libraries(vf-moduleinterface
    PUBLIC
    zeravalidator
    VeinMeta::VfEvent
    VeinMeta::VfComponent
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(vf-moduleinterface
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/vf-moduleinterface>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

set_target_properties(vf-moduleinterface PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(vf-moduleinterface PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(vf-moduleinterface)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/vf-moduleinterface_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/vf-moduleinterface
    )

install(TARGETS vf-moduleinterface
    EXPORT vf-moduleinterfaceExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

write_basic_package_version_file(
    vf-moduleinterfaceConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMajorVersion
    )

configure_file(vf-moduleinterfaceConfig.cmake.in vf-moduleinterfaceConfig.cmake @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/vf-moduleinterfaceConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/vf-moduleinterfaceConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/vf-moduleinterface
    )

install(EXPORT vf-moduleinterfaceExport
    FILE vf-moduleinterfaceTargets.cmake
    NAMESPACE ZeraClasses::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/vf-moduleinterface
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/vf-moduleinterface
    )
