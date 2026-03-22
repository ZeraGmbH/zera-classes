function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_link_libraries(${_testname}
            PRIVATE
            statusmodule-static
            Zera::zenuxservicecommon-demolib
            zera-modulemanager-testlib
            dspsupermodule-testlib
            Zera::VfFrameworkTest
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
    test_status_config_load
    test_readinstrumenttype
    test_statusmodule_with_dspsuper
)

target_sources(test_statusmodule_with_dspsuper
  PRIVATE
  test-data/test-data.qrc
)

target_sources(test_readinstrumenttype
  PRIVATE
  test-data/test-data.qrc
)
