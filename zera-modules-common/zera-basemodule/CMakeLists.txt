file(GLOB SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/declare-only/*.h
    )

add_library(zerabasemodule SHARED
    ${SOURCES}
    ${HEADER}
    )

target_link_libraries(zerabasemodule 
    PUBLIC
    MeasurementModuleInterface
    zeravalidator
    zeraxmlconfig
    zera-service-interfaces
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(zerabasemodule
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/basemodule>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/declare-only>
    )

set_target_properties(zerabasemodule PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zerabasemodule PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(zerabasemodule)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zerabasemodule_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/basemodule
    )

install(TARGETS zerabasemodule
    EXPORT zerabasemoduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

write_basic_package_version_file(
    zerabasemoduleConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMajorVersion
    )

configure_file(zerabasemoduleConfig.cmake.in zerabasemoduleConfig.cmake @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/zerabasemoduleConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/zerabasemoduleConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zerabasemodule
    )

install(EXPORT zerabasemoduleExport
    FILE zerabasemoduleTargets.cmake
    NAMESPACE ZeraClasses::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zerabasemodule
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/basemodule
    )
