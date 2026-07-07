file(GLOB SOURCES 
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

add_library(rmsmodule-lib SHARED
    ${SOURCES}
    ${HEADER}
)

target_include_directories(rmsmodule-lib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(rmsmodule-lib
    PUBLIC
    zera-basemodule
    service-common
    Zera::zera-math
    Zera::zera-timers
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

set_target_properties(rmsmodule-lib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(rmsmodule-lib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS rmsmodule-lib NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS rmsmodule-lib
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
