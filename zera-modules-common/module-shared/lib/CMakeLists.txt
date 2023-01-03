file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )
file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

add_library(module-shared SHARED
    ${HEADER}
    ${SOURCES}
    )

target_link_libraries(module-shared
    PUBLIC
    module-tasks
    )

target_include_directories(module-shared
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/module-shared>
    )

set_target_properties(module-shared PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(module-shared PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

install(TARGETS module-shared
    EXPORT zeramodulesharedExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

write_basic_package_version_file(
    zeramodulesharedConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMajorVersion
    )

configure_file(zeramodulesharedConfig.cmake.in zeramodulesharedConfig.cmake @ONLY)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/zeramodulesharedConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/zeramodulesharedConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zera-module-shared
    )

install(EXPORT zeramodulesharedExport
    FILE zeramodulesharedTargets.cmake
    NAMESPACE ZeraClasses::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zera-module-shared
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/module-shared
    )

