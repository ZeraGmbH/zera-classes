function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_sources(${_testname}
            PRIVATE
            ${XML_SCHEMA_RESOURCE_EFFICIENCY1MODULE}
        )
        target_link_libraries(${_testname}
            PRIVATE
            efficiency1module-static
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
    test_efficiency1_config_load
)

