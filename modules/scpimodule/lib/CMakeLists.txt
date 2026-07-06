file(GLOB SOURCES 
    *.cpp
    delegates/*.cpp
    delegates-measure/*.cpp
    scpi-clients/*.cpp
    scpi-groups/*.cpp
    )

file(GLOB HEADER
    *.h
    delegates/*.h
    delegates-measure/*.h
    scpi-clients/*.h
    scpi-groups/*.h
    )

add_library(scpimodule-lib SHARED
    ${SOURCES}
    ${HEADER}
    )

target_include_directories(scpimodule-lib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/delegates>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/delegates-measure>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/scpi-clients>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/scpi-groups>
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
