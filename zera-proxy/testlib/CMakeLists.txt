file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(zera-proxy-testlib STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(zera-proxy-testlib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(zera-proxy-testlib
    PUBLIC
    Qt5::Core
    zera-proxy
    service-interfaces
    zera-timers-testlib
)
