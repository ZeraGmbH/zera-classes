file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
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
    Qt5::Concurrent
    Zera::VfEvent
    Zera::VfComponent
    Zera::VfStorageHash
    Zera::VfNet2
    Zera::VfTcp
    Zera::VfQml
    Zera::VfCpp
    Zera::ModmanBase
    Zera::VfCryptoBridge
    Zera::resman-lib
    Zera::zera-timers-demolib
    Zera::zenuxservicecommon-demolib
    VeinMeta::VfScript
    VeinMeta::VfLogger
    VeinMeta::VfExport
    VeinMeta::VfFiles
    module-qt-plugin-interface
    zera-json-params
    )
