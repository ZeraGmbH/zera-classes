file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(module-actual-values SHARED
    ${SOURCES}
    ${HEADER}
    )

target_link_libraries(module-actual-values
    PUBLIC
    Qt5::Core
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(module-actual-values
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/module-actual-values>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

set_target_properties(module-actual-values PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(module-actual-values PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS module-actual-values NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS module-actual-values
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/module-actual-values
    )
