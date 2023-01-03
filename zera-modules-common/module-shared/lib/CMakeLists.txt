file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )
file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

add_library(module-shared SHARED
    ${HEADER}
    ${SOURCES}
    )

target_link_libraries(module-shared
    PUBLIC
    module-tasks
    )

target_include_directories(module-shared
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/module-shared>
    )

set_target_properties(module-shared PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(module-shared PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(module-shared)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/module-shared_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/module-shared
    )

export(TARGETS module-shared NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS module-shared
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/module-shared
    )

