file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/pcb-server/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/resman/*.h
    )
file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pcb-server/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resman/*.cpp
    )

add_library(module-tasks SHARED
    ${HEADER}
    ${SOURCES}
    )

target_link_libraries(module-tasks
    PUBLIC
    Zera::zera-tasks-base
    PRIVATE
    Zera::service-interfaces
    )

target_include_directories(module-tasks
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/pcb-server>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/resman>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/module-tasks>
    )

set_target_properties(module-tasks PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(module-tasks PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(module-tasks)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/module-tasks_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/module-tasks
    )

export(TARGETS module-tasks NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS module-tasks
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/module-tasks
    )
