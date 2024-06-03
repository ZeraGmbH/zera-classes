file(GLOB SOURCES
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

add_library(system-metrics-testlib STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(system-metrics-testlib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(system-metrics-testlib
    PUBLIC
    system-metrics
)
