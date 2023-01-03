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

add_library(module-tasks SHARED
    ${HEADER}
    ${SOURCES}
    )

target_link_libraries(module-tasks
    PUBLIC
    tasks-base
    zera-service-interfaces
    )

target_include_directories(module-tasks
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/pcb-server>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/resman>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/module-tasks>
    )

set_target_properties(module-tasks PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(module-tasks PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

install(TARGETS module-tasks
    EXPORT zeramoduletasksExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

write_basic_package_version_file(
    zeramoduletasksConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMajorVersion
    )

configure_file(zeramoduletasksConfig.cmake.in zeramoduletasksConfig.cmake @ONLY)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/zeramoduletasksConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/zeramoduletasksConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zera-module-tasks
    )

install(EXPORT zeramoduletasksExport
    FILE zeramoduletasksTargets.cmake
    NAMESPACE ZeraClasses::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zera-module-tasks
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/module-tasks
    )

