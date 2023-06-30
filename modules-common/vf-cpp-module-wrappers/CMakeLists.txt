file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(vf-cpp-module-wrappers SHARED
    ${SOURCES}
    ${HEADER}
    )

target_link_libraries(vf-cpp-module-wrappers
    PUBLIC
    vf-moduleinterface
    VeinMeta::VfCpp
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(vf-cpp-module-wrappers
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/vf-cpp-module-wrappers>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

set_target_properties(vf-cpp-module-wrappers PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(vf-cpp-module-wrappers PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(vf-cpp-module-wrappers)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/vf-cpp-module-wrappers_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/vf-cpp-module-wrappers
    )

export(TARGETS vf-cpp-module-wrappers NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS vf-cpp-module-wrappers
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/vf-cpp-module-wrappers
    )
