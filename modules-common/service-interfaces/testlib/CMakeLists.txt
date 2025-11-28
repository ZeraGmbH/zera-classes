file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(module-service-interfaces-testlib STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(module-service-interfaces-testlib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/module-service-interfaces-testlib>
    )

target_link_libraries(module-service-interfaces-testlib
    PUBLIC
    module-service-interfaces-demolib
)

set_target_properties(module-service-interfaces-testlib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(module-service-interfaces-testlib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS module-service-interfaces-testlib NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS module-service-interfaces-testlib
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/module-service-interfaces-testlib
    )
