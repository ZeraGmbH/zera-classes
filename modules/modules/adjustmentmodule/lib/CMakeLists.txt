file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tasks/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/tasks/*.h
    )

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
    zera-basemodule
    Zera::zera-math
    module-tasks
)

