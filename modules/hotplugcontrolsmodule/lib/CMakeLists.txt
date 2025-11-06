file(GLOB SOURCES
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

add_library(hotplugcontrolsmodule-static STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(hotplugcontrolsmodule-static
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(hotplugcontrolsmodule-static
    zera-basemodule
)

