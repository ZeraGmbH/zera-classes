file(GLOB SOURCES 
    *.cpp
    delegates/*.cpp
    scpi-clients/*.cpp
    )

file(GLOB HEADER
    *.h
    delegates/*.h
    scpi-clients/*.h
    )

add_library(scpimodule-static STATIC
    scpimodulexmlschema.qrc
    ${SOURCES}
    ${HEADER}
    )

target_include_directories(scpimodule-static
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/delegates>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/scpi-clients>
    )

target_link_libraries(scpimodule-static
    PUBLIC
    Qt5::SerialPort
    zera-basemodule
    Zera::SCPI
    Zera::zera-timers
    Zera::zenuxservicecommon
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

set_target_properties(scpimodule-static PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(scpimodule-static PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS scpimodule-static NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS scpimodule-static
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/scpimodule-static
    )
