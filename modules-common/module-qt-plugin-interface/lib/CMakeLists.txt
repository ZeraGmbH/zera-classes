file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC" )

add_library(module-qt-plugin-interface STATIC
    ${HEADER}
    ${SOURCES}
)

target_link_libraries(module-qt-plugin-interface
    PUBLIC
    ${QTX}::Core
    module-service-interfaces
    channel-range-observer
    Zera::VfTcp
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

if("${QT_MAJOR_VER}" STREQUAL "6")
    target_link_libraries(module-qt-plugin-interface
        PUBLIC
        ${QTX}::StateMachine
    )
endif()

target_include_directories(module-qt-plugin-interface
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/module-qt-plugin-interface>
    )

set_target_properties(module-qt-plugin-interface PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(module-qt-plugin-interface PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(module-qt-plugin-interface)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/module-qt-plugin-interface_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/module-qt-plugin-interface
    )

export(TARGETS module-qt-plugin-interface NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS module-qt-plugin-interface
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/module-qt-plugin-interface
    )
