file(GLOB SOURCES
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC" )

add_library(modemodule-static STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(modemodule-static
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(modemodule-static
    zera-basemodule
)

