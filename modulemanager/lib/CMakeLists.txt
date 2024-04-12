file(GLOB SOURCES
    *.cpp
    non-module-entities/*.cpp
    )

file(GLOB HEADER
    *.h
    non-module-entities/*.h
    )

add_library(zera-modulemanager-lib STATIC
    ${SOURCES}
    ${HEADER}
    )

target_include_directories(zera-modulemanager-lib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/non-module-entities>
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
    Zera::zenuxservicecommon-demolib
    VeinMeta::VfLogger
    VeinMeta::VfExport
    VeinMeta::VfFiles
    module-qt-plugin-interface
    zera-json-params
    module-service-interfaces
    module-service-interfaces-demolib
    )
