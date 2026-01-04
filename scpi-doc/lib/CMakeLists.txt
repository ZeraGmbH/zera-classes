file(GLOB SOURCES
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

add_library(scpi-doc-lib STATIC
    ${SOURCES}
    ${HEADER}
    )

target_include_directories(scpi-doc-lib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(scpi-doc-lib
    PUBLIC
    ${QTX}::Core
    zera-modulemanager-lib
    zera-modulemanager-testlib
    )

