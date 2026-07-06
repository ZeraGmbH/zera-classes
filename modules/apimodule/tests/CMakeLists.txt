function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_link_libraries(${_testname}
            PRIVATE
            zera-modulemanager-testlib
            apimodule-lib
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
    test_api_trust_authentication
)

target_sources(test_api_trust_authentication
  PRIVATE
    test-data/test-data.qrc
)
