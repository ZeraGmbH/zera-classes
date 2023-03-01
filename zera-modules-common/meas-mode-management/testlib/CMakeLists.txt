file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(zera-meas-mode-management-testlib STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(zera-meas-mode-management-testlib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(zera-meas-mode-management-testlib
    PUBLIC
    zera-meas-mode-management
)
