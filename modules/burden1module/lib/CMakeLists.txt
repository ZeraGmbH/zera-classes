file(GLOB SOURCES
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC" )

add_library(burden1module-static STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(burden1module-static
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(burden1module-static
    zera-basemodule
    Zera::zera-math
)

