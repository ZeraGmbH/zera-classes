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
    module-validators
    Zera::VfEvent
    Zera::VfComponent
    Zera::ModmanBase
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(vf-moduleinterface
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/vf-moduleinterface>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

set_target_properties(vf-moduleinterface PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(vf-moduleinterface PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(vf-moduleinterface)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/vf-moduleinterface_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/vf-moduleinterface
    )

export(TARGETS vf-moduleinterface NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS vf-moduleinterface
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/vf-moduleinterface
    )

add_subdirectory(tests)
