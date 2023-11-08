file(GLOB SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(modules-common-sec SHARED
    ${HEADER}
    ${SOURCES}
    )

target_link_libraries(modules-common-sec
    PUBLIC
    Zera::zera-tasks-base
    Zera::service-interfaces
    module-tasks
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(modules-common-sec
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/modules-common-sec>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )

set_target_properties(modules-common-sec PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(modules-common-sec PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(modules-common-sec)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/modules-common-sec_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/modules-common-sec
    )

export(TARGETS modules-common-sec NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS modules-common-sec
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/modules-common-sec
    )
