file(GLOB SOURCES 
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

add_library(dftmodule-static STATIC
    dftmodulexmlschema.qrc
    ${SOURCES}
    ${HEADER}
    )

target_include_directories(dftmodule-static
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(dftmodule-static
    PUBLIC
    zera-basemodule
    service-common
    Zera::zera-math
    Zera::zera-timers
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

export(TARGETS dftmodule-static NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS dftmodule-static
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dftmodule-static
    )
