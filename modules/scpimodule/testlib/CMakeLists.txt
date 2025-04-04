file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(scpimodule-testlib STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(scpimodule-testlib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(scpimodule-testlib
    PUBLIC
    scpimodule-static
    Zera::zera-timers-testlib
    Zera::VfStorage
    Zera::zera-timers-testlib
    zera-basemodule
)

set_target_properties(scpimodule-testlib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(scpimodule-testlib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS scpimodule-testlib NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS scpimodule-testlib
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/scpimodule-testlib
    )
