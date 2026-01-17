function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_link_libraries(${_testname}
            PRIVATE
            samplemodule-static
            zera-modulemanager-testlib
        )
        target_sources(${_testname}
          PRIVATE
            test-data/test-data.qrc
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
    test_sample_config_load
    test_sample_persistency
    test_sample_set_channel
    test_sample_range_module_value_observer
    test_sample_pll_automatic
    test_sample_set_pll_automatic
)
