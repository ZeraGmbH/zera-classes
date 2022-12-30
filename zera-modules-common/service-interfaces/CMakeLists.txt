file(GLOB SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(zera-service-interfaces SHARED
    ${HEADER}
    ${SOURCES}
    )

target_link_libraries(zera-service-interfaces
    PUBLIC
    Qt5::Core
    Qt5::Network
    zeraproxy
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(zera-service-interfaces
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-service-interfaces>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

set_target_properties(zera-service-interfaces PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zera-service-interfaces PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(zera-service-interfaces)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zera-service-interfaces_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-service-interfaces
    )

install(TARGETS zera-service-interfaces
    EXPORT zera-service-interfacesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

write_basic_package_version_file(
    zera-service-interfacesConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMajorVersion
    )

configure_file(zera-service-interfacesConfig.cmake.in zera-service-interfacesConfig.cmake @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/zera-service-interfacesConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/zera-service-interfacesConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zera-service-interfaces
    )

install(EXPORT zera-service-interfacesExport
    FILE zera-service-interfacesTargets.cmake
    NAMESPACE ZeraClasses::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zera-service-interfaces
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-service-interfaces
    )

