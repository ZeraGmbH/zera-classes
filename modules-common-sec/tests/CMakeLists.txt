function(SETUP_QTESTS_SEC_COMMON)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_link_libraries(${_testname}
            PRIVATE
            modules-common-sec
            zera-modulemanager-testlib
        )
    endforeach()
endfunction()

SETUP_QTESTS_SEC_COMMON(
    test_uint32expander
    test_secpowermoduleobserver
)

target_sources(test_secpowermoduleobserver
  PRIVATE
    test-data/test-data.qrc
)
