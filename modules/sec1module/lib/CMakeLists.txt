file(GLOB SOURCES
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC" )

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
    modules-common-sec
    Zera::zera-timers
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
)

