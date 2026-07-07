function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_link_libraries(${_testname}
            PRIVATE
            zera-modulemanager-testlib
            referencemodule-lib
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
)

