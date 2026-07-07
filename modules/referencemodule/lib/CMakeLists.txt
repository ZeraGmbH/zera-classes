file(GLOB SOURCES 
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

add_library(referencemodule-lib SHARED
    ${SOURCES}
    ${HEADER}
    )

target_include_directories(referencemodule-lib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(referencemodule-lib
    PUBLIC
    zera-basemodule
    Zera::zera-math
    Zera::zera-timers
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

set_target_properties(referencemodule-lib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(referencemodule-lib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS referencemodule-lib NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS referencemodule-lib
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
