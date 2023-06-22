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

list(JOIN SESSION_FILES "," C_SESSION_FILES)
add_definitions(-DSESSION_FILES="${C_SESSION_FILES}")

list(JOIN LOGGER_CONFIG_FILES "," C_LOGGER_CONFIG_FILES)
add_definitions(-DLOGGER_CONFIG_FILES="${C_LOGGER_CONFIG_FILES}")

set(JSON_CONFIG_LOADER_TEST_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/json-test-files/simple-valid.json
    )

add_custom_target(json-test-logger ALL
    SOURCES ${JSON_CONFIG_LOADER_TEST_FILES}
    COMMAND ${JSONLINT} ${JSON_CONFIG_LOADER_TEST_FILES}
    COMMAND ${JSONSCHEMA} -i ${JSON_CONFIG_LOADER_TEST_FILES} ${LOGGER_CONFIGS_SCHEMA} || (echo \"Error: Schema validation failed in '${JSON_CONFIG_LOADER_TEST_FILES}'\" 1>&2$<SEMICOLON> exit 1)
    )

set(JSON_SESSION_TEST_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/json-test-files/simple-valid-session.json
    )

add_custom_target(json-test-session ALL
    SOURCES ${JSON_SESSION_TEST_FILES}
    COMMAND ${JSONLINT} ${JSON_SESSION_TEST_FILES}
    COMMAND ${JSONSCHEMA} -i ${JSON_SESSION_TEST_FILES} ${SESSION_SCHEMA} || (echo \"Error: Schema validation failed in '${JSON_SESSION_TEST_FILES}'\" 1>&2$<SEMICOLON> exit 1)
    )

add_definitions(-DJSON_TEST_DIR="${CMAKE_CURRENT_SOURCE_DIR}/json-test-files")

function(SETUP_TESTS)
    foreach(_testname ${ARGN})
        add_test(NAME ${_testname}
                 COMMAND ${_testname})
        add_executable(${_testname}
            ${_testname}.h
            ${_testname}.cpp
            )
        target_include_directories(${_testname}
            PRIVATE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${PROJECT_SOURCE_DIR}/src
            )
        target_link_libraries(${_testname}
            PUBLIC
            testhelpers
            zera-modulemanager-lib
            )
    endforeach()
endfunction()

SETUP_TESTS(
    test_jsonloggercontentloader
    test_jsonconfigfiles
    test_loggercontentsessionloader
    )
