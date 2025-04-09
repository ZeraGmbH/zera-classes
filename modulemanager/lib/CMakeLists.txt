file(GLOB SOURCES
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

add_library(zera-modulemanager-lib STATIC
    ${SOURCES}
    ${HEADER}
    )

target_include_directories(zera-modulemanager-lib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-modulemanager-lib>
    )

target_link_libraries(zera-modulemanager-lib
    PUBLIC
    Qt5::Core
    Qt5::Network
    Zera::VfEvent
    Zera::VfComponent
    Zera::VfStorage
    Zera::VfNet2
    Zera::VfTcp
    Zera::VfCpp
    Zera::ModmanBase
    Zera::zenuxservicecommon-demolib
    Zera::service-interfaces
    VeinMeta::VfLogger
    VeinMeta::VfExport
    VeinMeta::VfFiles
    module-qt-plugin-interface
    zera-json-params
    module-service-interfaces
    module-service-interfaces-demolib
    )

export(TARGETS zera-modulemanager-lib NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS zera-modulemanager-lib
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-modulemanager-lib
    )
