file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(vf-cpp-module-wrappers SHARED
    ${SOURCES}
    ${HEADER}
    )

target_link_libraries(vf-cpp-module-wrappers
    PUBLIC
    zera-vein-moduleinterface
    VeinMeta::VfCpp
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(vf-cpp-module-wrappers
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/vf-cpp-module-wrappers>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

set_target_properties(vf-cpp-module-wrappers PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(vf-cpp-module-wrappers PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(vf-cpp-module-wrappers)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/vf-cpp-module-wrappers_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/vf-cpp-module-wrappers
    )

install(TARGETS vf-cpp-module-wrappers
    EXPORT vf-cpp-module-wrappersExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

write_basic_package_version_file(
    vf-cpp-module-wrappersConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMajorVersion
    )

configure_file(vf-cpp-module-wrappersConfig.cmake.in vf-cpp-module-wrappersConfig.cmake @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/vf-cpp-module-wrappersConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/vf-cpp-module-wrappersConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/vf-cpp-module-wrappers
    )

install(EXPORT vf-cpp-module-wrappersExport
    FILE vf-cpp-module-wrappersTargets.cmake
    NAMESPACE ZeraClasses::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/vf-cpp-module-wrappers
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/vf-cpp-module-wrappers
    )
