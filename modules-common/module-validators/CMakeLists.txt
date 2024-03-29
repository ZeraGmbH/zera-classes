file(GLOB SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(module-validators SHARED
    ${SOURCES}
    ${HEADER}
    )

target_link_libraries(module-validators
    PUBLIC
    Qt5::Core
    zera-json-params
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(module-validators
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/validator>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

set_target_properties(module-validators PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(module-validators PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(module-validators)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/module-validators_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/validator
    )

export(TARGETS module-validators NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS module-validators
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/validator
    )
