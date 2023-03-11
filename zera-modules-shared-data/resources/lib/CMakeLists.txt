file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(zera-resources SHARED
    ${SOURCES}
    ${HEADER}
    )

target_link_libraries(zera-resources
    PUBLIC
    module-tasks
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(zera-resources
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/zera-resources>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

set_target_properties(zera-resources PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zera-resources PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(zera-resources)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zera-resources_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/zera-resources
    )

export(TARGETS zera-resources NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS zera-resources
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/zera-resources
    )
