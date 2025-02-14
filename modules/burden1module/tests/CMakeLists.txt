function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_sources(${_testname}
            PRIVATE
            ${XML_SCHEMA_RESOURCE_BURDEN1MODULE}
        )
        target_link_libraries(${_testname}
            PRIVATE
            burden1module-static
            zera-modulemanager-testlib
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
    test_burden1_config_load
    test_burden1_module_regression
)

target_sources(test_burden1_module_regression
    PRIVATE
    test-data/test-data.qrc
)
