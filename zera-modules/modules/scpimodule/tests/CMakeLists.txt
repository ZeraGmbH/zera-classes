function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_sources(${_testname}
            PRIVATE
            ${XML_SCHEMA_RESOURCE}
        )
        target_link_libraries(${_testname}
            PRIVATE
            scpimodule-static
            scpimodule-testlib
            statusmodule-static
            zera-modules-testlib
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
    test_scpi_config_load
    test_scpi_cmds_in_session
)

