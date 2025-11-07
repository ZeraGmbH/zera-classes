function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_link_libraries(${_testname}
            PRIVATE
            sem1module-static
            zera-modulemanager-testlib
            Zera::xml-compare-testlib
            Zera::VfFrameworkTest
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
    test_sem1_config_load
)
