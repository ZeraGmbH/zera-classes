file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(thdnmodule-demolib STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(thdnmodule-demolib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/thdnmodule-demolib>
    )

target_link_libraries(thdnmodule-demolib
    PUBLIC
    thdnmodule-static
    module-service-interfaces
    Zera::service-interfaces-demolib
    Zera::zera-timers
)

set_target_properties(thdnmodule-demolib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(thdnmodule-demolib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS thdnmodule-demolib NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS thdnmodule-demolib
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/thdnmodule-demolib
    )
