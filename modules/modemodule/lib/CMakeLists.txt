file(GLOB SOURCES
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

add_library(modemodule-lib SHARED
    ${SOURCES}
    ${HEADER}
)

target_include_directories(modemodule-lib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(modemodule-lib
    PUBLIC
    zera-basemodule
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
)

set_target_properties(modemodule-lib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(modemodule-lib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS modemodule-lib NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS modemodule-lib
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
