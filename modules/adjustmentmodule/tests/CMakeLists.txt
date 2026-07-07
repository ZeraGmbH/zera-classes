function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_link_libraries(${_testname}
            PRIVATE
            Zera::VfConvenientCode
            adjustmentmodule-lib
            adjmodule-testlib
            rangemodule-lib
            dftmodule-lib
            fftmodule-lib
            rmsmodule-lib
            zera-modulemanager-testlib
            dsp-support-testlib
            scpimodule-testlib
            Zera::zenuxservicecommon-demolib
            Zera::zenuxservicecommon-testlib
            Zera::mt310s2d-testlib
            Zera::xml-compare-testlib
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
    test_adj_module_regression
    test_adj_module_gain
    test_adj_module_gain_dc
    test_adj_module_gain_dc_com5003_ref
    test_adj_module_phase
    test_adj_module_offset
    test_adj_module_lem_offset_sequences
    test_taskoffsetgetadjcorrection
    test_taskgaingetadjcorrection
)

target_sources(test_adj_module_regression
    PRIVATE
    test-data/test-data.qrc
)

target_sources(test_adj_module_gain
    PRIVATE
    test-data/test-data.qrc
)

target_sources(test_adj_module_lem_offset_sequences
    PRIVATE
    test-data/test-data.qrc
)

target_sources(test_adj_module_gain_dc
    PRIVATE
    test-data/test-data.qrc
)

target_sources(test_adj_module_gain_dc_com5003_ref
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
