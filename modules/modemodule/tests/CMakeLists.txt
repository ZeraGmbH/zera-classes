function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_sources(${_testname}
            PRIVATE
            ${XML_SCHEMA_RESOURCE_MODEMODULE}
        )
        target_link_libraries(${_testname}
            PRIVATE
            modemodule-static
            zera-modulemanager-testlib
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
    test_mode_config_load
    test_mode_regression
)

target_sources(test_mode_regression
  PRIVATE
    test-data/test-data.qrc
)
