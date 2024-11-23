function(SETUP_QTESTS_CHANNEL_MANAGER)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_link_libraries(${_testname}
            PUBLIC
            channel-range-observer
            Zera::zenuxservicecommon-testlib
            Zera::vf-tcp-mock-networklib
            Zera::mt310s2d-testlib
            )
    endforeach()
endfunction()

SETUP_QTESTS_CHANNEL_MANAGER(
    test_all_channels_observer
    test_channel_observer
)
