file(GLOB XML_CONFIGS
    *.xml
)

install(
    FILES ${XML_CONFIGS}
    DESTINATION ${SET_MODMAN_CONFIG_PATH}
    )

