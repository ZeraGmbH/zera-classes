function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_sources(${_testname}
            PRIVATE
            ${XML_SCHEMA_RESOURCE_DFTMODULE}
        )
        target_link_libraries(${_testname}
            PRIVATE
            zera-modulemanager-testlib
            dftmodule-static
            Zera::VfConvenientCode
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
    test_dft_config_load
    test_dft_module_regression
)

target_sources(test_dft_module_regression
  PRIVATE
    test-data/test-data.qrc
)
