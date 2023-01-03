file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )
file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

add_library(zera-tasks-base SHARED
    ${HEADER}
    ${SOURCES}
    )

target_link_libraries(zera-tasks-base
    PUBLIC
    Qt5::Core
    zera-timers
    )

target_include_directories(zera-tasks-base
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/tasks-base>
    )

set_target_properties(zera-tasks-base PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zera-tasks-base PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

install(TARGETS zera-tasks-base
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

generate_export_header(zera-tasks-base)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zera-tasks-base_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/tasks-base
    )

export(TARGETS zera-tasks-base FILE ZeraClassesTargets.cmake)
install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/tasks-base
    )

