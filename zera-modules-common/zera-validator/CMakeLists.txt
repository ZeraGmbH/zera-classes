file(GLOB SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(zeravalidator SHARED
    ${SOURCES}
    ${HEADER}
    )

target_link_libraries(zeravalidator
    PUBLIC
    zera-json-params
    zera-vein-moduleinterface
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(zeravalidator
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zeravalidator>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

set_target_properties(zeravalidator PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zeravalidator PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(zeravalidator)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zeravalidator_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zeravalidator
    )

install(TARGETS zeravalidator
    EXPORT zeravalidatorExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

write_basic_package_version_file(
    zeravalidatorConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMajorVersion
    )

configure_file(zeravalidatorConfig.cmake.in zeravalidatorConfig.cmake @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/zeravalidatorConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/zeravalidatorConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zeravalidator
    )

install(EXPORT zeravalidatorExport
    FILE zeravalidatorTargets.cmake
    NAMESPACE ZeraClasses::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zeravalidator
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zeravalidator
    )
