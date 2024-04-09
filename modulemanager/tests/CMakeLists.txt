# Inspired by
# https://github.com/KDAB/kdabtv/blob/master/Qt-Widgets-and-more/UnitTests/Tests/CMakeLists.txt

add_library(testhelpers STATIC
    )

target_include_directories(testhelpers
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(testhelpers
    PUBLIC
    Qt5::Test
    )

list(JOIN LOGGER_CONFIG_FILES "," C_LOGGER_CONFIG_FILES)
add_definitions(-DLOGGER_CONFIG_FILES="${C_LOGGER_CONFIG_FILES}")

set(JSON_CONFIG_LOADER_TEST_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/json-test-files/simple-valid.json
    )

add_custom_target(json-test-logger ALL
    SOURCES ${JSON_CONFIG_LOADER_TEST_FILES}
    COMMAND ${JSONLINT} ${JSON_CONFIG_LOADER_TEST_FILES}
    COMMAND export PYTHONWARNINGS='ignore' && ${JSONSCHEMA} -i ${JSON_CONFIG_LOADER_TEST_FILES} ${LOGGER_CONFIGS_SCHEMA} || (echo \"Error: Schema validation failed in '${JSON_CONFIG_LOADER_TEST_FILES}'\" 1>&2$<SEMICOLON> exit 1)
    )

set(JSON_SESSION_TEST_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/json-test-files/simple-valid-session.json
    )

add_custom_target(json-test-session ALL
    SOURCES ${JSON_SESSION_TEST_FILES}
    COMMAND ${JSONLINT} ${JSON_SESSION_TEST_FILES}
    COMMAND export PYTHONWARNINGS='ignore' && ${JSONSCHEMA} -i ${JSON_SESSION_TEST_FILES} ${SESSION_SCHEMA} || (echo \"Error: Schema validation failed in '${JSON_SESSION_TEST_FILES}'\" 1>&2$<SEMICOLON> exit 1)
    )

add_definitions(-DJSON_TEST_DIR="${CMAKE_CURRENT_SOURCE_DIR}/json-test-files")
add_definitions(-DOE_INSTALL_ROOT="${OE_INSTALL_ROOT}")

function(SETUP_QTESTS_MODULEMANAGER)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_link_libraries(${_testname}
            PUBLIC
            testhelpers
            zera-modulemanager-lib
            zera-modulemanager-testlib
            )
    endforeach()
endfunction()

SETUP_QTESTS_MODULEMANAGER(
    test_jsonloggercontentloader
    test_jsonconfigfiles
    test_loggercontentsessionloader
    test_modman_config
    test_plugin_load
    test_modman_regression_all_sessions
    test_sessionnamesmappingjson
    test_modman_with_vf_logger
    )

target_sources(test_modman_regression_all_sessions
  PRIVATE
    ${XML_SCHEMA_RESOURCES_ALL}
)

target_sources(test_modman_with_vf_logger
  PRIVATE
    ${XML_SCHEMA_RESOURCE_RMSMODULE}
    test-data/test-data.qrc
)
