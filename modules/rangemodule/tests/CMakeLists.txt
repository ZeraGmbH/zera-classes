function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_sources(${_testname}
            PRIVATE
            ${XML_SCHEMA_RESOURCE_RANGEMODULE}
        )
        target_link_libraries(${_testname}
            PRIVATE
            rangemodule-static
            zera-modulemanager-testlib
            Zera::zdsp1d-testlib
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
    test_range_config_load
    test_range_module_regression
    test_range_automatic
)

target_sources(test_range_config_load
  PRIVATE
)

target_sources(test_range_module_regression
  PRIVATE
    test-data/test-data.qrc
)

target_sources(test_range_automatic
  PRIVATE
    test-data/test-data.qrc
)
