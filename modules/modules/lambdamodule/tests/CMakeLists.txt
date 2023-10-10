function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_sources(${_testname}
            PRIVATE
            ${XML_SCHEMA_RESOURCE_LAMBDAMODULE}
        )
        target_link_libraries(${_testname}
            PRIVATE
            lambdamodule-static
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
    test_lambda_config_load
    test_lambda_calculator
)

