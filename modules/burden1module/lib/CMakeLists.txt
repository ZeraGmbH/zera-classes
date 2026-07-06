file(GLOB SOURCES
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

add_library(burden1module-lib SHARED
    ${SOURCES}
    ${HEADER}
)

target_include_directories(burden1module-lib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(burden1module-lib
    zera-basemodule
    Zera::zera-math
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
)

set_target_properties(burden1module-lib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(burden1module-lib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS burden1module-lib NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS burden1module-lib
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
