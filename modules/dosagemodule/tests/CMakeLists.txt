# Dumps dummy target to see them in IDE
file(GLOB_RECURSE TESTFILES
    test-data/*
)
# Unique target name!
add_custom_target(dosagemodule-test-data ALL
    SOURCES ${TESTFILES}
    )

function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_link_libraries(${_testname}
            PRIVATE
            dosagemodule-lib
            zera-modulemanager-testlib
            Zera::zdsp1d-testlib
            Zera::mt310s2d-testlib
        )
        target_compile_definitions(${_testname}
            PRIVATE
            TEST_SRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
    test_dosage_module_regression
    test_dosage
)

target_sources(test_dosage_module_regression
    PRIVATE
    test-data/test-data.qrc
)

target_sources(test_dosage
    PRIVATE
    test-data/test-data.qrc
)

