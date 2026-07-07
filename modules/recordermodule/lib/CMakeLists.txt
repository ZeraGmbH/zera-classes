file(GLOB SOURCES
    *.cpp
    rpc/*.cpp
    )

file(GLOB HEADER
    *.h
    rpc/*.h
    )

add_library(recordermodule-lib SHARED
    ${SOURCES}
    ${HEADER}
)

target_include_directories(recordermodule-lib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/rpc>
)

target_link_libraries(recordermodule-lib
    PUBLIC
    zera-basemodule
    vf-cpp-module-wrappers
    Zera::zera-jsonobject-to-string
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
)

set_target_properties(recordermodule-lib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(recordermodule-lib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS recordermodule-lib NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS recordermodule-lib
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
