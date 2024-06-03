function(SETUP_SYSTEM_METRICE_TESTS)
    foreach(_testname ${ARGN})
        SETUP_QTESTS(${_testname})
        target_link_libraries(${_testname}
            PUBLIC
            system-metrics
            system-metrics-testlib
        )
    endforeach()
endfunction()

SETUP_SYSTEM_METRICE_TESTS(
    test_procstatdecoder
)

target_sources(test_procstatdecoder
    PRIVATE
    test-data/test-data.qrc
)
