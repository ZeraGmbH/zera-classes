file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(zera-channel-range-info SHARED
    ${SOURCES}
    ${HEADER}
    )

target_link_libraries(zera-channel-range-info
    PUBLIC
    module-tasks
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(zera-channel-range-info
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/channel-range-info>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

set_target_properties(zera-channel-range-info PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zera-channel-range-info PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(zera-channel-range-info)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zera-channel-range-info_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/channel-range-info
    )

export(TARGETS zera-channel-range-info NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS zera-channel-range-info
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/channel-range-info
    )
