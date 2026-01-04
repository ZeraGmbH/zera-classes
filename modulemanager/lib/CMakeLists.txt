add_definitions(-DMODMAN_MODULE_PATH="${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/zera-modules/")

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
    ${QTX}::Core
    ${QTX}::Network
    Zera::VfEvent
    Zera::VfComponent
    Zera::VfStorage
    Zera::VfNet2
    Zera::VfTcp
    Zera::VfCpp
    Zera::ModmanBase
    Zera::service-interfaces
    Zera::zenux-device-info
    Zera::zera-json-params
    VeinMeta::VfLogger
    VeinMeta::VfExport
    VeinMeta::VfFiles
    module-qt-plugin-interface
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
