file(GLOB XML_CONFIGS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.xml
)

file(GLOB XML_SCHEMA
    ${CMAKE_CURRENT_SOURCE_DIR}/*.xsd
)

# check for valid config files / display in qt-creator
add_custom_target(rangemodule-configs ALL
    DEPENDS ${XML_SCHEMA} ${XML_CONFIGS}
    SOURCES ${XML_SCHEMA} ${XML_CONFIGS}
    COMMAND xmllint --noout --schema ${XML_SCHEMA} ${XML_CONFIGS}
    )

install(
    FILES ${XML_CONFIGS}
    DESTINATION ${SET_MODMAN_CONFIG_PATH}
    )

