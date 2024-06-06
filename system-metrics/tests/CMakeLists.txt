function(SETUP_SYSTEM_METRICE_TESTS)
    foreach(_testname ${ARGN})
        SETUP_QTESTS(${_testname})
        target_sources(${_testname}
            PRIVATE
            test-data/test-data.qrc
        )
        target_link_libraries(${_testname}
            PUBLIC
            system-metrics
            system-metrics-testlib
        )
    endforeach()
endfunction()

SETUP_SYSTEM_METRICE_TESTS(
    test_procstatdecoder
    test_cpuload
)
