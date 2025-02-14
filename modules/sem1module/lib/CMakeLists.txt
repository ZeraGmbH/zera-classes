file(GLOB SOURCES
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

add_library(sem1module-static STATIC
    sem1modulexmlschema.qrc
    ${SOURCES}
    ${HEADER}
)

target_include_directories(sem1module-static
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(sem1module-static
    zera-basemodule
    modules-common-sec
    Zera::zera-timers
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
)

