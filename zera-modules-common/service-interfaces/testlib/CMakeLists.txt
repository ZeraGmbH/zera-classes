file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(service-interfaces-testlib STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(service-interfaces-testlib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(service-interfaces-testlib
    PUBLIC
    service-interfaces
    zera-proxy-testlib
)

