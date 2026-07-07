file(GLOB SOURCES 
    *.cpp
    clients/*.cpp
    delegates-common/*.cpp
    scpi-models-basic/*.cpp
    scpi-ieee-488-2/*.cpp
    scpi-measure/*.cpp
    scpi-status/*.cpp
    )

file(GLOB HEADER
    *.h
    clients/*.h
    delegates-common/*.h
    scpi-models-basic/*.h
    scpi-ieee-488-2/*.h
    scpi-measure/*.h
    scpi-status/*.h
    )

add_library(scpimodule-lib SHARED
    ${SOURCES}
    ${HEADER}
    )

target_include_directories(scpimodule-lib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/clients>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/delegates-common>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/scpi-models-basic>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/scpi-ieee-488-2>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/scpi-measure>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/scpi-status>
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
