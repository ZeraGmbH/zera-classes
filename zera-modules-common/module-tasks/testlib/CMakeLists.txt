file(GLOB HELPER_SOURCES
    *.h
    *.cpp
)

add_library(module-tasks-testlib STATIC
    ${HELPER_SOURCES}
    )

target_include_directories(module-tasks-testlib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(module-tasks-testlib
    PUBLIC
    Qt5::Core
    Qt5::Network
    Qt5::Test
    module-tasks
    zeraproxy
    tasks-base
    zera-timers-testlib
    tasks-base-testlib
    Zera::SCPI
    scpi-testlib
    )

