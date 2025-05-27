add_definitions(-DOPERATOR_HOME="${OPERATOR_HOME}")
add_definitions(-DZC_SERVICES_IP="${ZC_SERVICES_IP}")
add_definitions(-DMODMAN_SESSION_PATH="${SET_MODMAN_SESSION_PATH}")
add_definitions(-DMODMAN_DEFAULT_SESSION="${SET_MODMAN_DEFAULT_SESSION}")
add_definitions(-DMODMAN_CONTENTSET_PATH="${SET_MODMAN_CONTENTSET_PATH}")
add_definitions(-DMODMAN_CUSTOMERDATA_PATH="${OPERATOR_HOME}/customerdata/")
add_definitions(-DMODMAN_LOGGER_LOCAL_PATH="${OPERATOR_HOME}/logger")
add_definitions(-DMODMAN_AUTOMOUNT_PATH="${SET_MODMAN_AUTOMOUNT_PATH}")
file(GLOB SESSION_FILES ${SET_SESSION_FILES_SOURCE_PATH}/*.json)
list(JOIN SESSION_FILES "," C_SESSION_FILES)
add_definitions(-DSESSION_FILES="${C_SESSION_FILES}")

set(CONFIG_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/modulemanager_config.json
    ${CMAKE_CURRENT_SOURCE_DIR}/modulemanager_config_dev.json
)

file(GLOB LOGGER_CONFIG_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/loggerconfigs/*session*.json
)

set(LOGGER_CONFIGS_SCHEMA ${CMAKE_CURRENT_SOURCE_DIR}/json-schemas/logger-configs-schema.json)
set(SESSION_SCHEMA ${CMAKE_CURRENT_SOURCE_DIR}/json-schemas/session-schema.json)

# targets to see files in qt-creator and check for validity
add_custom_target(json-schemas ALL
    SOURCES ${LOGGER_CONFIGS_SCHEMA} ${SESSION_SCHEMA}
    COMMAND ${JSONLINT} ${LOGGER_CONFIGS_SCHEMA} ${SESSION_SCHEMA}
    )

set(LOGGER_CONFIG_SCHEMA_CMDS)
foreach(config ${LOGGER_CONFIG_FILES})
  list(APPEND LOGGER_CONFIG_SCHEMA_CMDS
      COMMAND export PYTHONWARNINGS='ignore' && ${JSONSCHEMA} -i ${config} ${LOGGER_CONFIGS_SCHEMA} || (echo \"JSON validation failed in '${config}'\" 1>&2$<SEMICOLON> exit 1)
    )
endforeach()

add_custom_target(logger-config-files ALL
    SOURCES ${LOGGER_CONFIG_FILES}
    COMMAND ${JSONLINT} ${LOGGER_CONFIG_FILES}
    ${LOGGER_CONFIG_SCHEMA_CMDS}
    )

set(SESSION_SCHEMA_CMDS)
foreach(config ${SESSION_FILES})
  list(APPEND SESSION_SCHEMA_CMDS
      COMMAND export PYTHONWARNINGS='ignore' && ${JSONSCHEMA} -i ${config} ${SESSION_SCHEMA} || (echo \"JSON validation failed in '${config}'\" 1>&2$<SEMICOLON> exit 1)
    )
endforeach()

add_custom_target(session-files ALL
    SOURCES ${SESSION_FILES}
    COMMAND ${JSONLINT} ${SESSION_FILES}
    ${SESSION_SCHEMA_CMDS}
    )

add_custom_target(zera-modulemanager-config ALL
    SOURCES ${CONFIG_FILES}
    COMMAND ${JSONLINT} ${CONFIG_FILES}
    )

install(
    FILES ${SESSION_FILES}
    DESTINATION ${SET_MODMAN_SESSION_PATH}
    )

install(
    FILES ${CONFIG_FILES}
    DESTINATION ${SET_MODMAN_CONFIG_PATH}
    )

install(
    FILES ${LOGGER_CONFIG_FILES}
    DESTINATION ${SET_MODMAN_CONTENTSET_PATH}
    )

enable_testing()
add_subdirectory(app)
add_subdirectory(demolib)
add_subdirectory(lib)
add_subdirectory(systemd)
add_subdirectory(tests)
add_subdirectory(testlib)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
