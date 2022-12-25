file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )
file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

add_library(zera-timers SHARED
    ${HEADER}
    ${SOURCES}
    )

target_link_libraries(zera-timers
    PUBLIC
    Qt5::Core
    )

target_include_directories(zera-timers
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/zera-timers>
    )

set_target_properties(zera-timers PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zera-timers PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

install(TARGETS zera-timers
    EXPORT zeratimersExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

write_basic_package_version_file(
    zeratimersConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMajorVersion
    )

configure_file(zeratimersConfig.cmake.in zeratimersConfig.cmake @ONLY)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/zeratimersConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/zeratimersConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zera-zera-timers
    )

install(EXPORT zeratimersExport
    FILE zeratimersTargets.cmake
    NAMESPACE ZeraClasses::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zera-zera-timers
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/zera-timers
    )

