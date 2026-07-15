file(GLOB SOURCES 
    *.cpp
    clients/*.cpp
    delegates-common/*.cpp
    scpi-model-catalogs/*.cpp
    scpi-model-dev-iface/*.cpp
    scpi-model-ieee-488-2/*.cpp
    scpi-model-measure-and-friends/*.cpp
    scpi-model-parameters/*.cpp
    scpi-model-rpcs/*.cpp
    scpi-model-status/*.cpp
    scpi-models-common/*.cpp
    )

file(GLOB HEADER
    *.h
    clients/*.h
    delegates-common/*.h
    scpi-model-catalogs/*.h
    scpi-model-dev-iface/*.h
    scpi-model-ieee-488-2/*.h
    scpi-model-measure-and-friends/*.h
    scpi-model-parameters/*.h
    scpi-model-rpcs/*.h
    scpi-model-status/*.h
    scpi-models-common/*.h
    )

add_library(scpimodule-lib SHARED
    ${SOURCES}
    ${HEADER}
    )

target_include_directories(scpimodule-lib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/clients>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/scpi-model-dev-iface>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/delegates-common>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/scpi-model-catalogs>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/scpi-model-ieee-488-2>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/scpi-model-measure-and-friends>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/scpi-model-parameters>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/scpi-model-rpcs>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/scpi-model-status>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/scpi-models-common>
    )

target_link_libraries(scpimodule-lib
    PUBLIC
    ${QTX}::SerialPort
    zera-basemodule
    Zera::SCPI
    Zera::zera-timers
    Zera::zenuxservicecommon
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

set_target_properties(scpimodule-lib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(scpimodule-lib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS scpimodule-lib NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS scpimodule-lib
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
