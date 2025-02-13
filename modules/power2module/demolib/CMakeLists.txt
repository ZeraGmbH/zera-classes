file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(power2module-demolib STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(power2module-demolib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(power2module-demolib
    PUBLIC
    power2module-static
    module-service-interfaces
    Zera::service-interfaces-demolib
    Zera::zera-timers
)

set_target_properties(power2module-demolib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(power2module-demolib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS power2module-demolib NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS power2module-demolib
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/power2module-demolib
    )
