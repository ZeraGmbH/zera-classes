function(SETUP_QTESTS_CHANNEL_RANGE_OBSERVER)
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

SETUP_QTESTS_CHANNEL_RANGE_OBSERVER(
    test_cro_system_observer
    test_cro_channel
    test_cro_range
    test_cro_channelcurrentrange
)
