file(GLOB SOURCES
    *.cpp
    )

file(GLOB HEADER
    *.h
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
    Zera::zenuxservicecommon
    Zera::service-interfaces
)

