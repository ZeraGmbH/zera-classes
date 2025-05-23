file(GLOB XML_CONFIGS
    *.xml
)

file(GLOB XSD_SCHEMA
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib/*.xsd
    )
file(TO_NATIVE_PATH ${XSD_SCHEMA} XSD_SCHEMA_ABS)

add_custom_target(blemodule-configs ALL
    SOURCES ${XSD_SCHEMA} ${XML_CONFIGS}
    COMMAND xmllint --noout --schema ${XSD_SCHEMA_ABS} ${XML_CONFIGS}
    )

install(
    FILES ${XML_CONFIGS}
    DESTINATION ${SET_MODMAN_CONFIG_PATH}
    )
