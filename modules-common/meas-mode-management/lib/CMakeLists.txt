file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/phase-set-strategies/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/phase-set-strategies/*.h
    )

add_library(zera-meas-mode-management SHARED
    ${SOURCES}
    ${HEADER}
    )

target_link_libraries(zera-meas-mode-management
    PUBLIC
    zera-dsp-helper
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(zera-meas-mode-management
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/meas-mode-management>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/phase-set-strategies>
    )

set_target_properties(zera-meas-mode-management PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zera-meas-mode-management PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(zera-meas-mode-management)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zera-meas-mode-management_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/meas-mode-management
    )

export(TARGETS zera-meas-mode-management NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS zera-meas-mode-management
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/meas-mode-management
    )
