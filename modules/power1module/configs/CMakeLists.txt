file(GLOB XML_CONFIGS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.xml
    ${CMAKE_CURRENT_SOURCE_DIR}/ced/*.xml
    ${CMAKE_CURRENT_SOURCE_DIR}/emob/*.xml
    ${CMAKE_CURRENT_SOURCE_DIR}/lem_dc/*.xml
    ${CMAKE_CURRENT_SOURCE_DIR}/ref/*.xml
    ${CMAKE_CURRENT_SOURCE_DIR}/perphase/*.xml
)

add_custom_target( # for visibility in qt creator
    power1-module-config-xml
    SOURCES ${XML_CONFIGS}
)

install(
    FILES ${XML_CONFIGS}
    DESTINATION ${SET_MODMAN_CONFIG_PATH}
    )

