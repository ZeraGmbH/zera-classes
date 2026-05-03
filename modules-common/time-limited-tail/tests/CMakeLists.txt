function(SETUP_QTESTS_TIME_LIMITED_TAIL)
    foreach(_testname ${ARGN})
        add_test(NAME ${_testname}
                 COMMAND ${_testname})
        add_executable(${_testname}
            ${_testname}.h
            ${_testname}.cpp
            )
        target_link_libraries(${_testname}
            PUBLIC
            ${QTX}::Test
            Zera::zera-timers-testlib
            time-limited-tail
            )
    endforeach()
endfunction()

SETUP_QTESTS_TIME_LIMITED_TAIL(
    test_timelimitedtail
)
