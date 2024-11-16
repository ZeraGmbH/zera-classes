function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_sources(${_testname}
            PRIVATE
            ${XML_SCHEMA_RESOURCE_ADJUSTMENTMODULE}
            ${XML_SCHEMA_RESOURCE_RANGEMODULE}
            ${XML_SCHEMA_RESOURCE_DFTMODULE}
            ${XML_SCHEMA_RESOURCE_FFTMODULE}
            ${XML_SCHEMA_RESOURCE_RMSMODULE}
            ${XML_SCHEMA_RESOURCE_SCPIMODULE}
        )
        target_link_libraries(${_testname}
            PRIVATE
            zera-modulemanager-testlib
            adjustmentmodule-static
            module-testlib
            rangemodule-static
            dftmodule-static
            fftmodule-static
            rmsmodule-static
            Zera::VfConvenientCode
            scpimodule-testlib
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
    test_adjustment_config_load
    test_adj_module_regression
    test_adj_module_gain
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

target_sources(test_adj_module_phase
    PRIVATE
    test-data/test-data.qrc
)

target_sources(test_adj_module_offset
    PRIVATE
    test-data/test-data.qrc
)
