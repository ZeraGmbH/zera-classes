file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(statusmodule-static STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(statusmodule-static
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(statusmodule-static
    zera-basemodule
    Zera::zera-timers
)

