function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_link_libraries(${_testname}
            PRIVATE
            dosagemodule-static
            zera-modulemanager-testlib
            Zera::zdsp1d-testlib
            Zera::mt310s2d-testlib
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
    test_dosage_config_load
    test_dosage_module_regression
    test_dosage
)

target_sources(test_dosage_module_regression
    PRIVATE
    test-data/test-data.qrc
)

target_sources(test_dosage
    PRIVATE
    test-data/test-data.qrc
)

