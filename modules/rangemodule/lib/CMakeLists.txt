file(GLOB SOURCES
    *.cpp
    tasks/*.cpp
    )

file(GLOB HEADER
    *.h
    tasks/*.h
    )

add_library(rangemodule-static STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(rangemodule-static
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tasks>
    )

target_link_libraries(rangemodule-static
    zera-basemodule
    Zera::zera-timers
    Zera::zera-log-statistics
)

export(TARGETS rangemodule-static NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS rangemodule-static
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/rangemodule-static
    )
