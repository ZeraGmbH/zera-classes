file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(channel-range-observer SHARED
    ${SOURCES}
    ${HEADER}
)

target_link_libraries(channel-range-observer
    PUBLIC
    ${QTX}::Core
    module-tasks
    module-service-interfaces
    Zera::service-interfaces
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
)

target_include_directories(channel-range-observer
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/channel-range-observer>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

set_target_properties(channel-range-observer PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(channel-range-observer PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS channel-range-observer NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS channel-range-observer
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/channel-range-observer
    )
