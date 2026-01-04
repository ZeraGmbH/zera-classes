file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(sourcemodule-testlib STATIC
    ${SOURCES}
    ${HEADER}
    )

target_link_libraries(sourcemodule-testlib
    PUBLIC
    ${QTX}::Test
    sourcemodule-static
    Zera::VfStorage
    Zera::zera-timers-testlib
    )

target_include_directories(sourcemodule-testlib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )
