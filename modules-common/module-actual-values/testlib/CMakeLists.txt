file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(module-actual-values-testlib STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(module-actual-values-testlib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(module-actual-values-testlib
    PUBLIC
    module-actual-values
    Zera::zera-timers
)

set_target_properties(module-actual-values-testlib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(module-actual-values-testlib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS module-actual-values-testlib NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS module-actual-values-testlib
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
