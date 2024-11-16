function(SETUP_QTESTS_CHANNEL_MANAGER)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_link_libraries(${_testname}
            PUBLIC
            pcb-channel-manager
            )
    endforeach()
endfunction()

SETUP_QTESTS_CHANNEL_MANAGER(
    test_pcb_channel_manager
)
