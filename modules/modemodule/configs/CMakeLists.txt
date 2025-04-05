file(GLOB XML_CONFIGS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.xml
)

add_custom_target(modemodule-configs
    SOURCES ${XML_CONFIGS}
    )

install(
    FILES ${XML_CONFIGS}
    DESTINATION ${SET_MODMAN_CONFIG_PATH}
    )
