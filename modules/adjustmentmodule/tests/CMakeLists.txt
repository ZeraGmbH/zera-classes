function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_link_libraries(${_testname}
            PRIVATE
            zera-modulemanager-testlib
            adjustmentmodule-static
            rangemodule-static
            dftmodule-static
            fftmodule-static
            rmsmodule-static
            Zera::VfConvenientCode
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
    test_adjustment_config_load
    test_adj_module_regression
    test_adj_module_gain
    test_adj_module_phase
)

target_sources(test_adj_module_regression
    PRIVATE
    test-data/test-data.qrc
)

target_sources(test_adj_module_gain
    PRIVATE
    test-data/test-data.qrc
)

target_sources(test_adj_module_phase
    PRIVATE
    test-data/test-data.qrc
)
