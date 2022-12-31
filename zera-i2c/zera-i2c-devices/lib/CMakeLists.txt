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
    Qt5::Core
    zerai2c
    zeramath
    zeramisc
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(zera-i2c-devices
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/zera-i2c-devices>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

set_target_properties(zera-i2c-devices PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zera-i2c-devices PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(zera-i2c-devices)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zera-i2c-devices_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/zera-i2c-devices
    )

install(TARGETS zera-i2c-devices
    EXPORT zera-i2c-devicesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    zera-i2c-devicesConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMajorVersion
    )

configure_file(zera-i2c-devicesConfig.cmake.in zera-i2c-devicesConfig.cmake @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/zera-i2c-devicesConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/zera-i2c-devicesConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zera-i2c-devices
    )

install(EXPORT zera-i2c-devicesExport
    FILE zera-i2c-devicesTargets.cmake
    NAMESPACE ZeraClasses::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zera-i2c-devices
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/zera-i2c-devices
    )
