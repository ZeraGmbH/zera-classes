file(GLOB XML_CONFIGS
    *.xml
    ced/*.xml
    emob/*.xml
    lem_dc/*.xml
    ref/*.xml
    perphase/*.xml
)

file(GLOB XSD_SCHEMA
    *.xsd
    )

add_custom_target(power1module-configs ALL
    SOURCES ${XSD_SCHEMA} ${XML_CONFIGS}
    COMMAND xmllint --noout --schema ${XSD_SCHEMA} ${XML_CONFIGS}
    )

install(
    FILES ${XML_CONFIGS}
    DESTINATION ${SET_MODMAN_CONFIG_PATH}
    )
