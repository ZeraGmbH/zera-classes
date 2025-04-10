file(GLOB HELPER_SOURCES
    *.h
    *.cpp
)

add_library(adjmodule-testlib STATIC
    ${HELPER_SOURCES}
    )

target_include_directories(adjmodule-testlib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(adjmodule-testlib
    PUBLIC
    adjustmentmodule-static
    module-testlib
    zera-modulemanager-testlib
    module-service-interfaces-testlib
    )


