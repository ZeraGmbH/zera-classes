file(GLOB SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

file(GLOB RESOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.qrc
    )

add_library(zerabasemodule SHARED
    ${SOURCES}
    ${HEADER}
    ${RESOURCES}
    )

target_link_libraries(zerabasemodule 
    PUBLIC
    Qt5::Gui
    Qt5::Network
    MeasurementModuleInterface
    zeravalidator
    zeraxmlconfig
    zera-json-params
    VeinMeta::VfEvent
    VeinMeta::VfHelpers
    VeinMeta::VfComponent
    VeinMeta::VfCpp
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

# announce headers - target perspective
target_include_directories(zerabasemodule
    PUBLIC
    $<INSTALL_INTERFACE:include/zera-classes/basemodule>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

set_target_properties(zerabasemodule PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zerabasemodule PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

include(GenerateExportHeader)
generate_export_header(zerabasemodule)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zerabasemodule_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/basemodule
    )

install(TARGETS zerabasemodule
    EXPORT zerabasemoduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

# configure *ConfigVersion.cmake
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    zerabasemoduleConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMinorVersion
    )

# configure *Config.cmake
configure_file(zerabasemoduleConfig.cmake.in zerabasemoduleConfig.cmake @ONLY)

# install *Config(Version).cmake
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/zerabasemoduleConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/zerabasemoduleConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zerabasemodule
    )

# install targets cmake-files
install(EXPORT zerabasemoduleExport
    FILE zerabasemoduleTargets.cmake
    NAMESPACE ZeraClasses::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zerabasemodule
    )

# install public headers
install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/basemodule
    )
