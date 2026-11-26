# Dumps dummy target to see them in IDE
file(GLOB_RECURSE TESTFILES
    test-data/*
)
# Unique target name!
add_custom_target(rangemodule-test-data ALL
    SOURCES ${TESTFILES}
    )


function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_link_libraries(${_testname}
            PRIVATE
            rangemodule-lib
            rangemodule-testlib
            zera-modulemanager-testlib
            Zera::zdsp1d-testlib
            Zera::mt310s2d-testlib
            Zera::xml-compare-testlib
            Zera::zenuxservicecommon
        )
        target_compile_definitions(${_testname}
            PRIVATE
            TEST_SRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
    test_range_module_regression
    test_range_to_zero_clamp_cheating
    test_range_adjustment_correction
    test_range_scaling_factors
    test_range_automatic
    test_range_scpi
    test_range_hardware_overload
)

target_sources(test_range_module_regression
  PRIVATE
    test-data/test-data.qrc
)

target_sources(test_range_to_zero_clamp_cheating
  PRIVATE
    test-data/test-data.qrc
)

target_sources(test_range_adjustment_correction
  PRIVATE
    test-data/test-data.qrc
)

target_sources(test_range_scaling_factors
  PRIVATE
    test-data/test-data.qrc
)

target_sources(test_range_automatic
  PRIVATE
    test-data/test-data.qrc
)

target_sources(test_range_scpi
  PRIVATE
    test-data/test-data.qrc
)

target_link_libraries(test_range_scpi
  PRIVATE
    scpimodule-lib
    scpimodule-testlib
)

target_sources(test_range_hardware_overload
  PRIVATE
    test-data/test-data.qrc
)
