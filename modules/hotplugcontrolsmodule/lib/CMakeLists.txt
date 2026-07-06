file(GLOB SOURCES
    *.cpp
    rpc/*.cpp
    )

file(GLOB HEADER
    *.h
    rpc/*.h
    )

add_library(hotplugcontrolsmodule-lib SHARED
    ${SOURCES}
    ${HEADER}
)

target_include_directories(hotplugcontrolsmodule-lib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/rpc>
    )

target_link_libraries(hotplugcontrolsmodule-lib
    zera-basemodule
    service-common
    vf-cpp-module-wrappers
    Zera::zenuxservicecommon
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

set_target_properties(hotplugcontrolsmodule-lib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(hotplugcontrolsmodule-lib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS hotplugcontrolsmodule-lib NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS hotplugcontrolsmodule-lib
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
