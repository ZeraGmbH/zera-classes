file(GLOB SOURCES
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

add_library(rangemodule-testlib STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(rangemodule-testlib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/rangemodule-testlib>
    )

target_link_libraries(rangemodule-testlib
    PUBLIC
    ${QTX}::Test
    module-service-interfaces-testlib
    Zera::zenuxservicecommon-testlib
    Zera::service-interfaces-testlib
)

set_target_properties(rangemodule-testlib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(rangemodule-testlib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS rangemodule-testlib NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS rangemodule-testlib
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/rangemodule-testlib
    )
