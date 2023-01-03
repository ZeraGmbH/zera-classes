file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(module-interface STATIC
    ${HEADER}
    )

target_link_libraries(module-interface
    PUBLIC
    Qt5::Core
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(module-interface
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/module-interface>
    )

set_target_properties(module-interface PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(module-interface PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(module-interface)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/module-interface_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/module-interface
    )

export(TARGETS module-interface NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS module-interface
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/module-interface
    )
