file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(pcb-channel-manager SHARED
    ${SOURCES}
    ${HEADER}
)

target_link_libraries(pcb-channel-manager
    PUBLIC
    Qt5::Core
    module-tasks
    Zera::service-interfaces
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
)

target_include_directories(pcb-channel-manager
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/pcb-channel-manager>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

set_target_properties(pcb-channel-manager PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(pcb-channel-manager PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS pcb-channel-manager NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS pcb-channel-manager
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/pcb-channel-manager
    )
