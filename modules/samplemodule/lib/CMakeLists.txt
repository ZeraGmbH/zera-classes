file(GLOB SOURCES 
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

add_library(samplemodule-lib SHARED
    ${SOURCES}
    ${HEADER}
    )

target_include_directories(samplemodule-lib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(samplemodule-lib
    PUBLIC
    zera-basemodule
    service-common
    Zera::zera-math
    Zera::zera-timers
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

set_target_properties(samplemodule-lib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(samplemodule-lib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS samplemodule-lib NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS samplemodule-lib
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
