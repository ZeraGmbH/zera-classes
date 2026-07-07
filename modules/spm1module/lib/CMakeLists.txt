file(GLOB SOURCES
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

add_library(spm1module-lib SHARED
    ${SOURCES}
    ${HEADER}
)

target_include_directories(spm1module-lib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(spm1module-lib
    PUBLIC
    zera-basemodule
    modules-common-sec
    Zera::zera-timers
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
)

set_target_properties(spm1module-lib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(spm1module-lib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS spm1module-lib NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS spm1module-lib
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
