file(GLOB XML_CONFIGS
    *.xml
)

add_custom_target(adjutmentmodule-configs
    SOURCES ${XML_CONFIGS}
    )

install(
    FILES ${XML_CONFIGS}
    DESTINATION ${SET_MODMAN_CONFIG_PATH}
    )

