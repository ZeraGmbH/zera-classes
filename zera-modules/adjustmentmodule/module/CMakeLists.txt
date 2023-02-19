file(GLOB RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.qrc
    )

file(GLOB CONFIG_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.xml
)

file(GLOB CONFIG_VALIDATOR
    ${CMAKE_CURRENT_SOURCE_DIR}/*.xsd
)

# check for valid config files / display in qt-creator
add_custom_target(adjustmentmodule-configs ALL
    DEPENDS ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    SOURCES ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    COMMAND xmllint --noout --schema ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    )

add_library(adjustmentmodule MODULE
    ${RESOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/adjustmentmodulefactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/adjustmentmodulefactory.h
    )

target_link_libraries(adjustmentmodule
    PRIVATE
    adjustmentmodule-static
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(adjustmentmodule
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )

install(TARGETS adjustmentmodule
    EXPORT adjustmentmoduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/zera-modules
    )

install(
    FILES ${CONFIG_FILES} ${CONFIG_VALIDATOR}
    DESTINATION ${SET_ZC_CONFIG_PATH}
    )

