function(SETUP_QTESTS_BASEMODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_link_libraries(${_testname}
            PUBLIC
            zera-basemodule
            )
    endforeach()
endfunction()

SETUP_QTESTS_BASEMODULE(test_movingwindowfilter)
