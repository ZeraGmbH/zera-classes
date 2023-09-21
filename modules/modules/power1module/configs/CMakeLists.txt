file(GLOB XML_CONFIGS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.xml
    ${CMAKE_CURRENT_SOURCE_DIR}/ced/*.xml
    ${CMAKE_CURRENT_SOURCE_DIR}/emob/*.xml
    ${CMAKE_CURRENT_SOURCE_DIR}/lem_dc/*.xml
    ${CMAKE_CURRENT_SOURCE_DIR}/ref/*.xml
    ${CMAKE_CURRENT_SOURCE_DIR}/perphase/*.xml
)

file(GLOB XML_SCHEMA
    ${CMAKE_CURRENT_SOURCE_DIR}/*.xsd
)

# check for valid config files / display in qt-creator
add_custom_target(power1module-configs ALL
    DEPENDS ${XML_SCHEMA} ${XML_CONFIGS}
    SOURCES ${XML_SCHEMA} ${XML_CONFIGS}
    COMMAND xmllint --noout --schema ${XML_SCHEMA} ${XML_CONFIGS}
    )

install(
    FILES ${XML_CONFIGS}
    DESTINATION ${SET_MODMAN_CONFIG_PATH}
    )

