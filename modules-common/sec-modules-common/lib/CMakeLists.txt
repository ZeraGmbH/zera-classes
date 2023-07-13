file(GLOB SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(sec-modules-common SHARED
    ${HEADER}
    ${SOURCES}
    )

target_link_libraries(sec-modules-common
    PUBLIC
    Zera::zera-tasks-base
    module-tasks
    service-interfaces
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(sec-modules-common
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/sec-modules-common>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )

set_target_properties(sec-modules-common PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(sec-modules-common PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(sec-modules-common)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/sec-modules-common_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/sec-modules-common
    )

export(TARGETS sec-modules-common NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS sec-modules-common
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/sec-modules-common
    )
