function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_link_libraries(${_testname}
            PRIVATE
            sem1module-static
            zera-modulemanager-testlib
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
    test_sem1_config_load
    test_read_lock_state
)


target_sources(test_read_lock_state
  PRIVATE
    test-data/test-data.qrc
)
