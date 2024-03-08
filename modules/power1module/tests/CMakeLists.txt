function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_link_libraries(${_testname}
            PRIVATE
            power1module-static
            zera-modulemanager-testlib
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
    test_power1_config_load
    test_power1dspmodefunctioncatalog
    test_power1_module_regression
)

target_sources(test_power1_module_regression
  PRIVATE
    test-data/test-data.qrc
)
