file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(power2module-static STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(power2module-static
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(power2module-static
    zera-basemodule
    zera-meas-mode-management
    Zera::zera-timers
)

