file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(power1module-static STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(power1module-static
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(power1module-static
    zera-basemodule
    zera-meas-mode-management
    Zera::zera-timers
)

