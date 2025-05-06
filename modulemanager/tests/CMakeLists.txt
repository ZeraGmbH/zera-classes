list(JOIN LOGGER_CONFIG_FILES "," C_LOGGER_CONFIG_FILES)
add_definitions(-DLOGGER_CONFIG_FILES="${C_LOGGER_CONFIG_FILES}")

add_definitions(-DOE_INSTALL_ROOT="${OE_INSTALL_ROOT}")
add_definitions(-DHTML_DOCS_PATH_TEST="${CMAKE_CURRENT_BINARY_DIR}/")

# Inspired by
# https://github.com/KDAB/kdabtv/blob/master/Qt-Widgets-and-more/UnitTests/Tests/CMakeLists.txt
function(SETUP_QTESTS_MODULEMANAGER)
    foreach(_testname ${ARGN})
        add_test(NAME ${_testname} COMMAND ${_testname})
        add_executable(${_testname} ${_testname}.h ${_testname}.cpp)
        target_include_directories(${_testname}
            PRIVATE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
            ${PROJECT_SOURCE_DIR}/src
            )
        target_link_libraries(${_testname}
            PUBLIC
            -Wl,--whole-archive
            Zera::testlib-openfiles
            -Wl,--no-whole-archive

            Qt5::Test
            zera-modulemanager-lib
            zera-modulemanager-testlib
            scpi-doc-lib
            scpimodule-testlib
            )
    endforeach()
endfunction()

SETUP_QTESTS_MODULEMANAGER(
    test_change_session
    test_jsonconfigfiles
    test_modman_config
    test_modman_regression_all_sessions
    test_modman_regression_all_sessions_scpi
    test_modman_with_vf_logger
    test_plugin_load
    test_sessionnamesmappingjson
    )

target_sources(test_change_session
  PRIVATE
    test-data/test-data.qrc
)

target_sources(test_modman_regression_all_sessions
  PRIVATE
    test-data/test-data.qrc
)

target_sources(test_modman_regression_all_sessions_scpi
  PRIVATE
    test-data/test-data.qrc
)

target_sources(test_modman_with_vf_logger
  PRIVATE
    test-data/test-data.qrc
)
