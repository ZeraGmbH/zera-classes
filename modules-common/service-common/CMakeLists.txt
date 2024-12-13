file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(service-common SHARED
    ${SOURCES}
    ${HEADER}
)

target_link_libraries(service-common
    PUBLIC
    channel-range-observer
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
)

target_include_directories(service-common
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/service-common>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

set_target_properties(service-common PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(service-common PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS service-common NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS service-common
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/service-common
    )
