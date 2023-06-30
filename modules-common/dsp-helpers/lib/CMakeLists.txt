file(GLOB SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(zera-dsp-helper SHARED
    ${SOURCES}
    ${HEADER}
    )

target_link_libraries(zera-dsp-helper
    PUBLIC
    Qt5::Core
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(zera-dsp-helper
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/dsp-helper>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

set_target_properties(zera-dsp-helper PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zera-dsp-helper PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(zera-dsp-helper)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zera-dsp-helper_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/dsp-helper
    )

export(TARGETS zera-dsp-helper NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS zera-dsp-helper
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/dsp-helper
    )
