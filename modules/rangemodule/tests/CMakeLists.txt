function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_link_libraries(${_testname}
            PRIVATE
            rangemodule-static
            zera-modulemanager-testlib
            Zera::zdsp1d-testlib
            Zera::mt310s2d-testlib
            Zera::xml-compare-testlib
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
    test_range_config_load
    test_range_module_regression
    test_range_automatic
    test_range_scpi
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

target_sources(test_range_scpi
  PRIVATE
    test-data/test-data.qrc
    ${XML_SCHEMA_RESOURCE_SCPIMODULE}
)

target_link_libraries(test_range_scpi
  PRIVATE
    scpimodule-static
    scpimodule-testlib
)
