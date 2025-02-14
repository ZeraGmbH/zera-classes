function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_sources(${_testname}
            PRIVATE
            test-data/test-data.qrc
        )
        target_link_libraries(${_testname}
            PRIVATE
            scpimodule-static
            statusmodule-static
            rangemodule-static
            scpimodule-testlib
            zera-modulemanager-testlib
            Zera::xml-compare-testlib
            Zera::zera-timers-testlib
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
    test_scpi_config_load
    test_scpi_cmds_in_session
    test_scpi_queue
    test_scpi_xsession_change
)
