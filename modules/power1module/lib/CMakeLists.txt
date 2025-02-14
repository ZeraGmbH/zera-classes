file(GLOB SOURCES
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

add_library(power1module-static STATIC
    power1modulexmlschema.qrc
    ${SOURCES}
    ${HEADER}
)

target_include_directories(power1module-static
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(power1module-static
    zera-basemodule
    zera-meas-mode-management
    Zera::zera-timers
)

export(TARGETS power1module-static NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS power1module-static
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/power1module-static
    )
