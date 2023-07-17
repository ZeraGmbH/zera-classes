function(SETUP_QTESTS_SEC_COMMON)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_link_libraries(${_testname}
            PRIVATE
            modules-common-sec
        )
    endforeach()
endfunction()

SETUP_QTESTS_SEC_COMMON(
    test_taskgetrefinputconstant
)
