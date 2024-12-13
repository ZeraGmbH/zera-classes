file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(module-service-interfaces-demolib STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(module-service-interfaces-demolib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(module-service-interfaces-demolib
    PUBLIC
    module-service-interfaces
    dftmodule-demolib
    fftmodule-demolib
    rangemodule-demolib
    rmsmodule-demolib
    samplemodule-demolib
    power1module-demolib
    power2module-demolib
    thdnmodule-demolib
    oscimodule-demolib
    Zera::service-interfaces-demolib
    Zera::zera-timers
)

set_target_properties(module-service-interfaces-demolib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(module-service-interfaces-demolib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
