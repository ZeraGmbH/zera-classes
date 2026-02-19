function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_link_libraries(${_testname}
            PRIVATE
            zera-modulemanager-testlib
            referencemodule-static
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
    test_reference_config_load
)

