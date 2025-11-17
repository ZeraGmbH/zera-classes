function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_link_libraries(${_testname}
            PRIVATE
            statusmodule-static
            Zera::zenuxservicecommon-demolib
            zera-modulemanager-testlib
            Zera::VfFrameworkTest
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
    test_status_config_load
    test_readinstrumenttype
)

target_sources(test_readinstrumenttype
  PRIVATE
  test-data/test-data.qrc
)

