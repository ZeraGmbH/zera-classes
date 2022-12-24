file(GLOB SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(MeasurementModuleInterface SHARED
    ${HEADER}
    )

target_link_libraries(MeasurementModuleInterface 
    PRIVATE
    Qt5::Core
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(MeasurementModuleInterface
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-modules>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

set_target_properties(MeasurementModuleInterface PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(MeasurementModuleInterface PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

install(TARGETS MeasurementModuleInterface
    EXPORT MeasurementModuleInterfaceExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    MeasurementModuleInterfaceConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMajorVersion
    )

configure_file(MeasurementModuleInterfaceConfig.cmake.in MeasurementModuleInterfaceConfig.cmake @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/MeasurementModuleInterfaceConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/MeasurementModuleInterfaceConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MeasurementModuleInterface
    )

install(EXPORT MeasurementModuleInterfaceExport
    FILE MeasurementModuleInterfaceTargets.cmake
    NAMESPACE ZeraClasses::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MeasurementModuleInterface
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-modules
    )
