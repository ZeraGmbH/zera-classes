file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(fftmodule-demolib STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(fftmodule-demolib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(fftmodule-demolib
    PUBLIC
    fftmodule
    module-service-interfaces
    Zera::service-interfaces-demolib
    Zera::zera-timers
)

set_target_properties(fftmodule-demolib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(fftmodule-demolib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
