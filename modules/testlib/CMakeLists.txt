file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(zera-modules-testlib STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(zera-modules-testlib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(zera-modules-testlib
    PUBLIC
    zera-basemodule
    Zera::VfStorageHash
    Zera::zera-timers-testlib
)

set_target_properties(zera-modules-testlib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zera-modules-testlib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
