function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_link_libraries(${_testname}
            PRIVATE
            hotplugcontrolsmodule-static
            zera-modulemanager-testlib
            rangemodule-static
            Zera::xml-compare-testlib
            Zera::VfFrameworkTest
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
    test_emob_vein_scpi
    test_hotplug_detection
)

target_sources(test_emob_vein_scpi
  PRIVATE
    test-data/test-data.qrc
)

target_sources(test_hotplug_detection
  PRIVATE
    test-data/test-data.qrc
)
