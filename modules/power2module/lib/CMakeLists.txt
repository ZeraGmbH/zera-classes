file(GLOB SOURCES
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

add_library(power2module-lib SHARED
    ${SOURCES}
    ${HEADER}
)

target_include_directories(power2module-lib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(power2module-lib
    PUBLIC
    zera-basemodule
    zera-meas-mode-management
    Zera::zera-timers
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
)

set_target_properties(power2module-lib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(power2module-lib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS power2module-lib NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS power2module-lib
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
