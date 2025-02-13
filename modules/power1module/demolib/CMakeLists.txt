file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(power1module-demolib STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(power1module-demolib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(power1module-demolib
    PUBLIC
    power1module-static
    module-service-interfaces
    Zera::service-interfaces-demolib
    Zera::zera-timers
)

set_target_properties(power1module-demolib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(power1module-demolib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS power1module-demolib NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS power1module-demolib
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/power1module-demolib
    )
