file(GLOB SOURCES
    *.cpp
    rpc/*.cpp
    )

file(GLOB HEADER
    *.h
    rpc/*.h
    )

add_library(sem1module-static STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(sem1module-static
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/rpc>
    )

target_link_libraries(sem1module-static
    zera-basemodule
    modules-common-sec
    vf-cpp-module-wrappers
    Zera::zera-timers
    Zera::VfCpp
    Zera::zenuxservicecommon
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
)

