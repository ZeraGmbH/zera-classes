function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_link_libraries(${_testname}
            PRIVATE
            recordermodule-static
            zera-modulemanager-testlib
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
    test_recorder
    test_recorder_config_load
    test_recorder_scpi
)

target_sources(test_recorder
    PRIVATE
    test-data/test-data.qrc
)

target_sources(test_recorder_scpi
    PRIVATE
    test-data/test-data.qrc
)
