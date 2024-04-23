file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(dftmodule-demolib STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(dftmodule-demolib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(dftmodule-demolib
    PUBLIC
    dftmodule-static
    module-service-interfaces
    Zera::service-interfaces-demolib
    Zera::zera-timers
)

set_target_properties(dftmodule-demolib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(dftmodule-demolib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
