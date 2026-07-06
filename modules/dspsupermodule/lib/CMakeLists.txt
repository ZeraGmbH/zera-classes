file(GLOB SOURCES 
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

add_library(dspsupermodule-lib SHARED
    ${SOURCES}
    ${HEADER}
    )

target_include_directories(dspsupermodule-lib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(dspsupermodule-lib
    PUBLIC
    zera-basemodule
    service-common
    Zera::zera-timers
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

set_target_properties(dspsupermodule-lib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(dspsupermodule-lib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS dspsupermodule-lib NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS dspsupermodule-lib
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
