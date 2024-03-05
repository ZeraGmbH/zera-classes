function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_link_libraries(${_testname}
            PRIVATE
            power1module-static
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
    test_power1_config_load
    test_power1dspmodefunctioncatalog
)

