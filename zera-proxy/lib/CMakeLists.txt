file(GLOB SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(zera-proxy SHARED
    ${SOURCES}
    ${HEADER}
    )

target_link_libraries(zera-proxy
    PUBLIC
    Qt5::Network
    VeinMeta::xiqnet
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(zera-proxy
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/proxy>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

set_target_properties(zera-proxy PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zera-proxy PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(zera-proxy)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zera-proxy_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/proxy
    )

export(TARGETS zera-proxy NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS zera-proxy
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/proxy
    )
