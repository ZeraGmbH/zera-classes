file(GLOB SOURCES 
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC" )

add_library(oscimodule-static STATIC
    ${SOURCES}
    ${HEADER}
    )

target_include_directories(oscimodule-static
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(oscimodule-static
    PUBLIC
    zera-basemodule
    service-common
    Zera::zera-math
    Zera::zera-timers
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

export(TARGETS oscimodule-static NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS oscimodule-static
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/oscimodule-static
    )
