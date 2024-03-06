file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(adjustmentmodule-demolib STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(adjustmentmodule-demolib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(adjustmentmodule-demolib
    PUBLIC
    dftmodule-demolib
    fftmodule-demolib
    rangemodule-demolib
    rmsmodule-demolib
)

set_target_properties(adjustmentmodule-demolib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(adjustmentmodule-demolib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
