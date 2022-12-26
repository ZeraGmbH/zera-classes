file(GLOB HELPER_SOURCES
    *.h
    *.cpp
)

add_library(task-test-helpers STATIC
    ${HELPER_SOURCES}
    )

target_include_directories(task-test-helpers
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(task-test-helpers
    PUBLIC
    Qt5::Core
    Qt5::Network
    Qt5::Test
    zeramisc
    zeraproxy
    tasks-base
    zera-timers-testlib
    Zera::SCPI
    )

