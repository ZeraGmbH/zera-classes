function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_link_libraries(${_testname}
            PRIVATE
            Zera::VfConvenientCode
            adjustmentmodule-static
            adjmodule-testlib
            rangemodule-static
            dftmodule-static
            fftmodule-static
            rmsmodule-static
            zera-modulemanager-testlib
            scpimodule-testlib
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
    test_adjustment_config_load
    test_adj_module_regression
    test_adj_module_gain
    test_adj_module_gain_dc
    test_adj_module_phase
    test_adj_module_offset
)

target_sources(test_adj_module_regression
    PRIVATE
    test-data/test-data.qrc
)

target_sources(test_adj_module_gain
    PRIVATE
    test-data/test-data.qrc
)

target_sources(test_adj_module_gain_dc
    PRIVATE
    test-data/test-data.qrc
)

target_sources(test_adj_module_phase
    PRIVATE
    test-data/test-data.qrc
)

target_sources(test_adj_module_offset
    PRIVATE
    test-data/test-data.qrc
)
