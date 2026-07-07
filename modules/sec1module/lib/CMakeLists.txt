file(GLOB SOURCES
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

add_library(sec1module-lib SHARED
    ${SOURCES}
    ${HEADER}
)

target_include_directories(sec1module-lib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(sec1module-lib
    zera-basemodule
    modules-common-sec
    Zera::zera-timers
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
)

set_target_properties(sec1module-lib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(sec1module-lib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS sec1module-lib NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS sec1module-lib
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
