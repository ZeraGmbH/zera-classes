file(GLOB SOURCES
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

add_library(transformer1module-lib SHARED
    ${SOURCES}
    ${HEADER}
)

target_include_directories(transformer1module-lib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(transformer1module-lib
    PUBLIC
    zera-basemodule
    Zera::zera-math
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
)

set_target_properties(transformer1module-lib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(transformer1module-lib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS transformer1module-lib NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS transformer1module-lib
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
