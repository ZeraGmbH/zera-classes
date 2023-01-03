file(GLOB SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(zera-i2c-devices SHARED
    ${SOURCES}
    ${HEADER}
    )

target_link_libraries(zera-i2c-devices
    PUBLIC
    zera-i2c
    zera-intel-hex
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(zera-i2c-devices
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/i2c-devices>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

set_target_properties(zera-i2c-devices PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zera-i2c-devices PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(zera-i2c-devices)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zera-i2c-devices_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/i2c-devices
    )

export(TARGETS zera-i2c-devices NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS zera-i2c-devices
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/i2c-devices
    )
