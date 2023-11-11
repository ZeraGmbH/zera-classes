file(GLOB SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/delegates/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/scpi-clients/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/delegates/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/scpi-clients/*.h
    )

add_library(scpimodule-static STATIC
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

