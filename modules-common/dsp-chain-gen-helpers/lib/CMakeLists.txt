file(GLOB SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(dsp-chain-gen-helpers SHARED
    ${SOURCES}
    ${HEADER}
    )

target_link_libraries(dsp-chain-gen-helpers
    PUBLIC
    ${QTX}::Core
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(dsp-chain-gen-helpers
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/dsp-helper>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

set_target_properties(dsp-chain-gen-helpers PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(dsp-chain-gen-helpers PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(dsp-chain-gen-helpers)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/dsp-chain-gen-helpers_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/dsp-helper
    )

export(TARGETS dsp-chain-gen-helpers NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS dsp-chain-gen-helpers
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/dsp-helper
    )
