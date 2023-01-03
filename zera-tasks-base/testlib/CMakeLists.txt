file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(tasks-base-testlib STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(tasks-base-testlib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(tasks-base-testlib
    PUBLIC
    Qt5::Core
    zera-tasks-base
    zera-timers-testlib
)
