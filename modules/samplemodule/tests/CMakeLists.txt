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
    test_sample_regression
)

target_sources(test_sample_regression
  PRIVATE
    test-data/test-data.qrc
)
