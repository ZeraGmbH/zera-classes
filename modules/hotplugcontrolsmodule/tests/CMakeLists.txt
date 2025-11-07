function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_link_libraries(${_testname}
            PRIVATE
            hotplugcontrolsmodule-static
            zera-modulemanager-testlib
            Zera::xml-compare-testlib
            Zera::VfFrameworkTest
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
    test_read_emob_lock_state
)

target_sources(test_read_emob_lock_state
  PRIVATE
    test-data/test-data.qrc
)
