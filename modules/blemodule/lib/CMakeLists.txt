file(GLOB SOURCES 
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

add_library(blemodule-static STATIC
    ${SOURCES}
    ${HEADER}
    )

target_include_directories(blemodule-static
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(blemodule-static
    PUBLIC
    zera-basemodule
    zera-meas-mode-management
    Zera::zera-math
    Zera::ble-sensors
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

