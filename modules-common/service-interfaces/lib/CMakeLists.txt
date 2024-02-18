file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(module-service-interfaces SHARED
    ${SOURCES}
    ${HEADER}
)

target_link_libraries(module-service-interfaces
    PUBLIC
    Qt5::Core
    Zera::service-interfaces
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(module-service-interfaces
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/module-service-interfaces>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

set_target_properties(module-service-interfaces PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(module-service-interfaces PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS module-service-interfaces NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS module-service-interfaces
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/module-service-interfaces
    )
