function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_link_libraries(${_testname}
            PRIVATE
            zera-modulemanager-testlib
            dspsupermodule-testlib
            Zera::VfConvenientCode
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
    test_dspsuper_config_load
    test_dspsuper_module
    test_dspsuper_global_integration_time
)

target_sources(test_dspsuper_config_load
  PRIVATE
    test-data/test-data.qrc
)

target_sources(test_dspsuper_global_integration_time
  PRIVATE
    test-data/test-data.qrc
)
