file(GLOB SOURCES
    *.cpp
    rpc/*.cpp
    )

file(GLOB HEADER
    *.h
    rpc/*.h
    )

add_library(statusmodule-lib SHARED
    ${SOURCES}
    ${HEADER}
)

target_include_directories(statusmodule-lib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(statusmodule-lib
    PUBLIC
    zera-basemodule
    vf-cpp-module-wrappers
    Zera::zera-timers
    Zera::zenuxservicecommon
    Zera::service-interfaces
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
)

set_target_properties(statusmodule-lib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(statusmodule-lib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS statusmodule-lib NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS statusmodule-lib
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
