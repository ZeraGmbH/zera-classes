file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(rangemodule-demolib STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(rangemodule-demolib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(rangemodule-demolib
    PUBLIC
    rangemodule-static
    module-service-interfaces
    Zera::service-interfaces-demolib
    Zera::zera-timers
)

set_target_properties(rangemodule-demolib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(rangemodule-demolib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
