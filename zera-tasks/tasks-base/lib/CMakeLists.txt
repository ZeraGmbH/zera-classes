file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )
file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

add_library(tasks-base SHARED
    ${HEADER}
    ${SOURCES}
    )

target_link_libraries(tasks-base
    PUBLIC
    Qt5::Core
    )

target_include_directories(tasks-base
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/tasks-base>
    )

set_target_properties(tasks-base PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(tasks-base PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

install(TARGETS tasks-base
    EXPORT zeratasksbaseExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

write_basic_package_version_file(
    zeratasksbaseConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMajorVersion
    )

configure_file(zeratasksbaseConfig.cmake.in zeratasksbaseConfig.cmake @ONLY)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/zeratasksbaseConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/zeratasksbaseConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zera-tasks-base
    )

install(EXPORT zeratasksbaseExport
    FILE zeratasksbaseTargets.cmake
    NAMESPACE ZeraClasses::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zera-tasks-base
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/tasks-base
    )

