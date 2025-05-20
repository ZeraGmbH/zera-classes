file(GLOB SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/io-device/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/io-queue/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/message-texts/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/module-gluelogic/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source-device/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source-protocols/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source-scanner/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/transaction-ids/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vein/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/io-device/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/io-queue/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/message-texts/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/module-gluelogic/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/source-device/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/source-protocols/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/source-scanner/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/transaction-ids/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/vein/*.h
    )

file(GLOB RESOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.qrc
    )

add_library(sourcemodule-static STATIC
    ${SOURCES}
    ${HEADER}
    ${RESOURCES}
    )

target_link_libraries(sourcemodule-static
    PUBLIC
    vf-cpp-module-wrappers
    zera-basemodule
    serialportasyncblock
    Zera::source-control-common-lib
    Zera::zera-timers
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(sourcemodule-static
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/io-device>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/io-queue>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/message-texts>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/module-gluelogic>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/source-device>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/source-protocols>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/source-scanner>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/transaction-ids>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/vein>
    )
