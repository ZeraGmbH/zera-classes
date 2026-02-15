file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(periodaveragemodule-demolib STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(periodaveragemodule-demolib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/periodaveragemodule-demolib>
    )

target_link_libraries(periodaveragemodule-demolib
    PUBLIC
    periodaveragemodule-static
    module-service-interfaces-demolib
    Zera::zera-timers
)

set_target_properties(periodaveragemodule-demolib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(periodaveragemodule-demolib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS periodaveragemodule-demolib NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS periodaveragemodule-demolib
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/periodaveragemodule-demolib
    )
