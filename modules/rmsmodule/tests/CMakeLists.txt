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
    test_act_values_rms
    test_regression_vein_module_rms
)

target_sources(test_act_values_rms
  PRIVATE
    test-data/test-data.qrc
)

target_sources(test_regression_vein_module_rms
  PRIVATE
    test-data/test-data.qrc
)
