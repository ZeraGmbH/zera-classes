file(GLOB HELPER_SOURCES
    *.h
    *.cpp
)

add_library(tasks-for-modules-testlib STATIC
    ${HELPER_SOURCES}
    )

target_include_directories(tasks-for-modules-testlib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(tasks-for-modules-testlib
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

