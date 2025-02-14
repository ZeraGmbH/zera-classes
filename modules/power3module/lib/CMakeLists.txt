file(GLOB SOURCES
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

add_library(power3module-static STATIC
    power3modulexmlschema.qrc
    ${SOURCES}
    ${HEADER}
)

target_include_directories(power3module-static
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(power3module-static
    zera-basemodule
)

