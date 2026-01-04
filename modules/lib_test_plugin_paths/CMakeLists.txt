add_compile_definitions(MODULE_PLUGIN_BUILD_PATH="${MODULE_PLUGIN_BUILD_PATH}")

file(GLOB SOURCES
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

add_library(test-plugin-paths STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(test-plugin-paths
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(test-plugin-paths
    ${QTX}::Core
)

set_target_properties(test-plugin-paths PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(test-plugin-paths PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS test-plugin-paths NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS test-plugin-paths
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
