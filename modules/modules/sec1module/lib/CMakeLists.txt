file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(sec1module-static STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(sec1module-static
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(sec1module-static
    zera-basemodule
    sec-modules-common
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
)

