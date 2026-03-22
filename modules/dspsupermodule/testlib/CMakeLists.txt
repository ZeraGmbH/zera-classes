file(GLOB SOURCES
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

add_library(dspsupermodule-testlib STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(dspsupermodule-testlib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/dspsupermodule-testlib>
    )

target_link_libraries(dspsupermodule-testlib
    PUBLIC
    dspsupermodule-static
    Zera::service-interfaces-testlib
)

set_target_properties(dspsupermodule-testlib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(dspsupermodule-testlib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS dspsupermodule-testlib NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS dspsupermodule-testlib
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dspsupermodule-testlib
    )
