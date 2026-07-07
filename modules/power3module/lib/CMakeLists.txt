file(GLOB SOURCES
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

add_library(power3module-lib SHARED
    ${SOURCES}
    ${HEADER}
)

target_include_directories(power3module-lib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(power3module-lib
    PUBLIC
    zera-basemodule
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
)

set_target_properties(power3module-lib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(power3module-lib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS power3module-lib NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS power3module-lib
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
