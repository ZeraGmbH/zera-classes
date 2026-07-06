function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_link_libraries(${_testname}
            PRIVATE
            blemodule-lib
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
)

