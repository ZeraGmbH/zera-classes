function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_sources(${_testname}
            PRIVATE
            ${XML_SCHEMA_RESOURCE_SEC1MODULE}
        )
        target_link_libraries(${_testname}
            PRIVATE
            sec1module-static
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
    test_sec1_config_load
)

