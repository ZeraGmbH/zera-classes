file(GLOB XML_CONFIGS
    *.xml
)

file(GLOB XSD_SCHEMA
    *.xsd
    )

add_custom_target(fftmodule-configs ALL
    SOURCES ${XSD_SCHEMA} ${XML_CONFIGS}
    COMMAND xmllint --noout --schema ${XSD_SCHEMA} ${XML_CONFIGS}
    )

install(
    FILES ${XML_CONFIGS}
    DESTINATION ${SET_MODMAN_CONFIG_PATH}
    )
