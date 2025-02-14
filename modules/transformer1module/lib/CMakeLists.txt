file(GLOB SOURCES
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

add_library(transformer1module-static STATIC
    transformer1modulexmlschema.qrc
    ${SOURCES}
    ${HEADER}
)

target_include_directories(transformer1module-static
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(transformer1module-static
    zera-basemodule
    Zera::zera-math
)

