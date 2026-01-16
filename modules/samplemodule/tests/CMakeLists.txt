function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_link_libraries(${_testname}
            PRIVATE
            samplemodule-static
            zera-modulemanager-testlib
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
    test_sample_config_load
    test_sample_persistency
    test_sample_set_channel
    test_sample_range_module_value_observer
)

target_sources(test_sample_config_load
  PRIVATE
    test-data/test-data.qrc
)

target_sources(test_sample_persistency
  PRIVATE
    test-data/test-data.qrc
)

target_sources(test_sample_set_channel
  PRIVATE
    test-data/test-data.qrc
)

target_sources(test_sample_range_module_value_observer
  PRIVATE
    test-data/test-data.qrc
)
