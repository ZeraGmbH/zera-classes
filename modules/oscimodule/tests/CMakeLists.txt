function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_sources(${_testname}
            PRIVATE
            ${XML_SCHEMA_RESOURCE_OSCIMODULE}
        )
        target_link_libraries(${_testname}
            PRIVATE
            zera-modulemanager-testlib
            oscimodule-static
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
    test_osci_config_load
    test_osci_module_regression
)

target_sources(test_osci_module_regression
  PRIVATE
    test-data/test-data.qrc
)
