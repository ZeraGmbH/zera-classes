file(GLOB SOURCES
    *.cpp
    tasks/*.cpp
    )

file(GLOB HEADER
    *.h
    tasks/*.h
    )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC" )

add_library(adjustmentmodule-static STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(adjustmentmodule-static
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tasks>
    )

target_link_libraries(adjustmentmodule-static
    PUBLIC
    zera-basemodule
    Zera::zera-math
    module-tasks
)

