function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_link_libraries(${_testname}
            PRIVATE
            statusmodule-static
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
    test_status_config_load
)

