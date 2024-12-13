function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_sources(${_testname}
            PRIVATE
            ${XML_SCHEMA_RESOURCE_POWER2MODULE}
        )
        target_link_libraries(${_testname}
            PRIVATE
            power2module-static
            zera-modulemanager-testlib
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
    test_power2_config_load
    test_power2_module_regression
)

target_sources(test_power2_module_regression
  PRIVATE
    test-data/test-data.qrc
)
