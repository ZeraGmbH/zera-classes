file(GLOB SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/notification-handler/*.cpp
)

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/declare-only/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/notification-handler/*.h
)

add_library(zera-basemodule SHARED
    ${SOURCES}
    ${HEADER}
    )

target_link_libraries(zera-basemodule
    PUBLIC
    module-qt-plugin-interface
    vf-moduleinterface
    module-service-interfaces
    Zera::service-interfaces
    Zera::zenux-device-info
    Zera::VfCpp
    PRIVATE
    Zera::zera-xmlconfig
    Zera::zera-timers
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(zera-basemodule
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/basemodule>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/declare-only>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/notification-handler>
)

set_target_properties(zera-basemodule PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zera-basemodule PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(zera-basemodule)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zera-basemodule_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/basemodule
    )

export(TARGETS zera-basemodule NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS zera-basemodule
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/basemodule
    )
