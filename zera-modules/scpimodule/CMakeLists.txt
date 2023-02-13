file(GLOB SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/delegates/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/scpi-clients/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/delegates/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/scpi-clients/*.h
    )

file(GLOB RESOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.qrc
    )

file(GLOB CONFIG_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.xml
)

file(GLOB CONFIG_VALIDATOR
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.xsd
)

# check for valid config files / display in qt-creator
add_custom_target(scpimodule-configs ALL
    DEPENDS ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    SOURCES ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    COMMAND xmllint --noout --schema ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    )

add_library(scpimodule-static STATIC
    ${SOURCES}
    ${HEADER}
    )

target_include_directories(scpimodule-static
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/delegates>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/scpi-clients>
    )

target_link_libraries(scpimodule-static
    PUBLIC
    Qt5::SerialPort
    zera-basemodule
    Zera::SCPI
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

add_library(scpimodule MODULE
    ${RESOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/module-factory/scpimodulefactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/module-factory/scpimodulefactory.h
    )

target_link_libraries(scpimodule
    PRIVATE
    scpimodule-static
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(scpimodule
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

install(TARGETS scpimodule
    EXPORT scpimoduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/zera-modules
    )

install(
    FILES ${CONFIG_FILES} ${CONFIG_VALIDATOR}
    DESTINATION ${SET_ZC_CONFIG_PATH}
    )
