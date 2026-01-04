file(GLOB SOURCES
    *.cpp
    rpc/*.cpp
    )

file(GLOB HEADER
    *.h
    rpc/*.h
    )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC" )

add_library(recordermodule-static STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(recordermodule-static
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/rpc>
)

target_link_libraries(recordermodule-static
    zera-basemodule
    vf-cpp-module-wrappers
)

