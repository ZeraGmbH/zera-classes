file(GLOB SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(dftmodule-static STATIC
    ${SOURCES}
    ${HEADER}
    )

target_include_directories(dftmodule-static
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(dftmodule-static
    PUBLIC
    zera-basemodule
    service-common
    Zera::zera-math
    Zera::zera-timers
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

