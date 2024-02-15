function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_sources(${_testname}
            PRIVATE
            ${XML_SCHEMA_RESOURCE_RMSMODULE}
        )
        target_link_libraries(${_testname}
            PRIVATE
            zera-modulemanager-testlib
            rmsmodule-static
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
    test_rms_config_load
    test_rms_module_regression
)

target_sources(test_rms_module_regression
  PRIVATE
    test-data/test-data.qrc
)
