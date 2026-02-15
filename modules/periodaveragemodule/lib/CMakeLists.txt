file(GLOB SOURCES 
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

add_library(periodaveragemodule-static STATIC
    ${SOURCES}
    ${HEADER}
    )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC" )

target_include_directories(periodaveragemodule-static
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(periodaveragemodule-static
    PUBLIC
    zera-basemodule
    service-common
    Zera::zera-timers
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

export(TARGETS periodaveragemodule-static NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS periodaveragemodule-static
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/periodaveragemodule-static
    )
