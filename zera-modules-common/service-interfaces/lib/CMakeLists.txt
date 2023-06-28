file(GLOB SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/proxy/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/proxy/*.h
    )

add_library(service-interfaces SHARED
    ${HEADER}
    ${SOURCES}
    )

target_link_libraries(service-interfaces
    PUBLIC
    Qt5::Network
    Zera::xiqnet
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(service-interfaces
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/service-interfaces>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/proxy>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )

set_target_properties(service-interfaces PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(service-interfaces PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(service-interfaces)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/service-interfaces_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/service-interfaces
    )

export(TARGETS service-interfaces NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS service-interfaces
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/service-interfaces
    )
