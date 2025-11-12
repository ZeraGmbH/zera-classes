file(GLOB SOURCES
    *.cpp
    rpc/*.cpp
    )

file(GLOB HEADER
    *.h
    rpc/*.h
    )

add_library(hotplugcontrolsmodule-static STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(hotplugcontrolsmodule-static
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/rpc>
    )

target_link_libraries(hotplugcontrolsmodule-static
    zera-basemodule
    service-common
    vf-cpp-module-wrappers
    Zera::zenuxservicecommon
)

