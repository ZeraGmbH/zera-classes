file(GLOB SOURCES 
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC" )

add_library(fftmodule-static STATIC
    ${SOURCES}
    ${HEADER}
    )

target_include_directories(fftmodule-static
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(fftmodule-static
    PUBLIC
    zera-basemodule
    service-common
    Zera::zera-math
    Zera::zera-timers
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

export(TARGETS fftmodule-static NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS fftmodule-static
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/fftmodule-static
    )
