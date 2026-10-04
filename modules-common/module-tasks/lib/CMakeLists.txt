file(GLOB HEADER
    *.h
    dsp-server/*.h
    pcb-server/*.h
    pcb-server/channels/*.h
    pcb-server/emob/*.h
    pcb-server/generator/*.h
    pcb-server/ranges/*.h
    pcb-server/sec/*.h
    resman-server/*.h
    )
file(GLOB SOURCES
    *.cpp
    dsp-server/*.cpp
    pcb-server/*.cpp
    pcb-server/channels/*.cpp
    pcb-server/emob/*.cpp
    pcb-server/generator/*.cpp
    pcb-server/ranges/*.cpp
    pcb-server/sec/*.cpp
    resman-server/*.cpp
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
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/dsp-server>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/pcb-server>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/pcb-server/channels>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/pcb-server/emob>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/pcb-server/generator>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/pcb-server/ranges>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/pcb-server/sec>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/resman-server>
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
