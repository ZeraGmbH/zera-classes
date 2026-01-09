file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(zera-modulemanager-testlib STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(zera-modulemanager-testlib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-modulemanager-testlib>
    )

add_definitions(-DOE_INSTALL_ROOT="${OE_INSTALL_ROOT}")

target_link_libraries(zera-modulemanager-testlib
    PUBLIC
    ${QTX}::Core
    test-plugin-paths
    Zera::zera-timers-testlib
    Zera::zenuxservicecommon-testlib
    zera-modulemanager-lib
    scpimodule-static
    scpimodule-testlib
    module-service-interfaces-demolib
    module-service-interfaces-testlib
    module-testlib
    Zera::VfConvenientCode
    Zera::veinserver-testlib
    Zera::xsession-change-demolib
    Zera::VfFrameworkTest
    VeinMeta::vf-logger-testlib
)

set_target_properties(zera-modulemanager-testlib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zera-modulemanager-testlib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS zera-modulemanager-testlib NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS zera-modulemanager-testlib
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-modulemanager-testlib
    )
