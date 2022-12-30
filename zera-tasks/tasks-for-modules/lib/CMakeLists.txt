file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/pcb-server/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/resman/*.h
    )
file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pcb-server/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resman/*.cpp
    )

add_library(tasks-for-modules SHARED
    ${HEADER}
    ${SOURCES}
    )

target_link_libraries(tasks-for-modules
    PUBLIC
    tasks-base
    zera-service-interfaces
    )

target_include_directories(tasks-for-modules
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/pcb-server>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/resman>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/tasks-for-modules>
    )

set_target_properties(tasks-for-modules PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(tasks-for-modules PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

install(TARGETS tasks-for-modules
    EXPORT zeratasksformodulesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

write_basic_package_version_file(
    zeratasksformodulesConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMajorVersion
    )

configure_file(zeratasksformodulesConfig.cmake.in zeratasksformodulesConfig.cmake @ONLY)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/zeratasksformodulesConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/zeratasksformodulesConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zera-tasks-for-modules
    )

install(EXPORT zeratasksformodulesExport
    FILE zeratasksformodulesTargets.cmake
    NAMESPACE ZeraClasses::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zera-tasks-for-modules
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/tasks-for-modules
    )

