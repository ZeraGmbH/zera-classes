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

add_definitions(-DOE_INSTALL_ROOT="${OE_INSTALL_ROOT}")

# Inspired by
# https://github.com/KDAB/kdabtv/blob/master/Qt-Widgets-and-more/UnitTests/Tests/CMakeLists.txt
function(SETUP_QTESTS_MODULEMANAGER)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_link_libraries(${_testname}
            PUBLIC
            testhelpers
            zera-modulemanager-lib
            zera-modulemanager-testlib
            scpi-doc-lib
            scpimodule-testlib
            )
    endforeach()
endfunction()

SETUP_QTESTS_MODULEMANAGER(
    test_jsonconfigfiles
    test_modman_config
    test_plugin_load
    test_modman_regression_all_sessions
    test_modman_with_vf_logger
    test_change_session
    test_sessionnamesmappingjson
    )

target_sources(test_modman_regression_all_sessions
  PRIVATE
    ${XML_SCHEMA_RESOURCES_ALL}
    test-data/test-data.qrc
)

target_sources(test_modman_with_vf_logger
  PRIVATE
    ${XML_SCHEMA_RESOURCE_RMSMODULE}
    test-data/test-data.qrc
)

target_sources(test_change_session
  PRIVATE
  ${XML_SCHEMA_RESOURCES_ALL}
)

