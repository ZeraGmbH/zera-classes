file(GLOB SOURCES 
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

add_library(thdnmodule-lib SHARED
    ${SOURCES}
    ${HEADER}
    )

target_include_directories(thdnmodule-lib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(thdnmodule-lib
    PUBLIC
    zera-basemodule
    service-common
    Zera::zera-math
    Zera::zera-timers
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

set_target_properties(thdnmodule-lib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(thdnmodule-lib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS thdnmodule-lib NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS thdnmodule-lib
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
