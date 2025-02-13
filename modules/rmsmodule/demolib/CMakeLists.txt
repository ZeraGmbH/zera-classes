file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(rmsmodule-demolib STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(rmsmodule-demolib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(rmsmodule-demolib
    PUBLIC
    rmsmodule-static
    module-service-interfaces
    Zera::service-interfaces-demolib
    Zera::zera-timers
)

set_target_properties(rmsmodule-demolib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(rmsmodule-demolib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS rmsmodule-demolib NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS rmsmodule-demolib
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/rmsmodule-demolib
    )
