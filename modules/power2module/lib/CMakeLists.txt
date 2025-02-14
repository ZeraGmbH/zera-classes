file(GLOB SOURCES
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

add_library(power2module-static STATIC
    power2modulexmlschema.qrc
    ${SOURCES}
    ${HEADER}
)

target_include_directories(power2module-static
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(power2module-static
    zera-basemodule
    zera-meas-mode-management
    Zera::zera-timers
)

export(TARGETS power2module-static NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS power2module-static
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/power2module-static
    )
