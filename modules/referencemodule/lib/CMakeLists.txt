file(GLOB SOURCES 
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC" )

add_library(referencemodule-static STATIC
    ${SOURCES}
    ${HEADER}
    )

target_include_directories(referencemodule-static
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(referencemodule-static
    PUBLIC
    zera-basemodule
    Zera::zera-math
    Zera::zera-timers
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

