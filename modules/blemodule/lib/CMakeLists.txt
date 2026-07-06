file(GLOB SOURCES 
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

add_library(blemodule-lib SHARED
    ${SOURCES}
    ${HEADER}
    )

target_include_directories(blemodule-lib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(blemodule-lib
    PUBLIC
    zera-basemodule
    zera-meas-mode-management
    Zera::zera-math
    Zera::ble-sensors
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

set_target_properties(blemodule-lib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(blemodule-lib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS blemodule-lib NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS blemodule-lib
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
