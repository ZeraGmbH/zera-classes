function(SETUP_QTESTS_BASE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_link_libraries(${_testname}
            PUBLIC
            testhelpers
            zera-modulemanager-testlib
            )
    endforeach()
endfunction()

SETUP_QTESTS_BASE(
    test_veindatacollector
)

target_sources(test_veindatacollector
  PRIVATE
    ${XML_SCHEMA_RESOURCE_RMSMODULE}
    ${XML_SCHEMA_RESOURCE_POWER1MODULE}
    test-data/test-data.qrc
)
