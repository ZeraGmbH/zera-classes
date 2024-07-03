file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(module-testlib STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(module-testlib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(module-testlib
    PUBLIC
    dftmodule-demolib
    fftmodule-demolib
    rangemodule-demolib
    rmsmodule-demolib
)

set_target_properties(module-testlib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(module-testlib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
