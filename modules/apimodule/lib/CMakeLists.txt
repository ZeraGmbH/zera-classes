file(GLOB SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(apimodule-lib SHARED
    ${SOURCES}
    ${HEADER}
    )

target_include_directories(apimodule-lib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(apimodule-lib
    PUBLIC
    vf-cpp-module-wrappers
    zera-basemodule
    zera-meas-mode-management
    Zera::zera-math
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

set_target_properties(apimodule-lib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(apimodule-lib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS apimodule-lib NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS apimodule-lib
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
