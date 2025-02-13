file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(samplemodule-demolib STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(samplemodule-demolib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(samplemodule-demolib
    PUBLIC
    samplemodule-static
    module-service-interfaces
    Zera::service-interfaces-demolib
    Zera::zera-timers
)

set_target_properties(samplemodule-demolib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(samplemodule-demolib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS samplemodule-demolib NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS samplemodule-demolib
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/samplemodule-demolib
    )
