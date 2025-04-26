file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(zera-modulemanager-demolib STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(zera-modulemanager-demolib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-modulemanager-demolib>
    )

target_link_libraries(zera-modulemanager-demolib
    PUBLIC
    zera-modulemanager-lib
    Zera::zenuxservicecommon-demolib
    module-service-interfaces-demolib
)

set_target_properties(zera-modulemanager-demolib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zera-modulemanager-demolib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS zera-modulemanager-demolib NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS zera-modulemanager-demolib
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-modulemanager-demolib
    )
