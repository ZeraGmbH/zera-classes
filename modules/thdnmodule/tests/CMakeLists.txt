function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_link_libraries(${_testname}
            PRIVATE
            zera-modulemanager-testlib
            thdnmodule-static
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
    test_thdn_config_load
    test_thdn_module_regression
)

target_sources(test_thdn_module_regression
  PRIVATE
    test-data/test-data.qrc
)
