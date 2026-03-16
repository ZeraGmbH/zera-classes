function(SETUP_QTESTS_DSP_CHAIN_GEN)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_link_libraries(${_testname}
            PUBLIC
            zera-basemodule
            )
    endforeach()
endfunction()

SETUP_QTESTS_DSP_CHAIN_GEN(
    test_inttohexstringconvert
    test_dspchainidgen
    test_dspatomiccommandgen
)
