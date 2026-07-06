function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_link_libraries(${_testname}
            PRIVATE
            zera-modulemanager-testlib
            periodaveragemodule-lib
            Zera::VfConvenientCode
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
    test_periodaverage_demovalues
    test_periodaverage_module
)

target_sources(test_periodaverage_module
  PRIVATE
    test-data/test-data.qrc
)
