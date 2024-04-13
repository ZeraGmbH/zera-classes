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
    )

target_link_libraries(zera-modulemanager-lib
    PUBLIC
    Qt5::Core
    Qt5::Network
    Zera::VfEvent
    Zera::VfComponent
    Zera::VfStorageHash
    Zera::VfNet2
    Zera::VfTcp
    Zera::VfQml
    Zera::VfCpp
    Zera::ModmanBase
    Zera::zenuxservicecommon-demolib
    VeinMeta::VfLogger
    VeinMeta::VfExport
    VeinMeta::VfFiles
    module-qt-plugin-interface
    zera-json-params
    module-service-interfaces
    module-service-interfaces-demolib
    )
