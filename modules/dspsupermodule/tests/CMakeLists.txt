# Dumps dummy target to see them in IDE
file(GLOB_RECURSE TESTFILES
    test-data/*
)
# Unique target name!
add_custom_target(dspsupermodule-test-data ALL
    SOURCES ${TESTFILES}
    )

function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_link_libraries(${_testname}
            PRIVATE
            zera-modulemanager-testlib
            dspsupermodule-testlib
            Zera::VfConvenientCode
        )
        target_compile_definitions(${_testname}
            PRIVATE
            TEST_SRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
    test_dspsuper_module
    test_dspsuper_global_integration_time
)

target_sources(test_dspsuper_module
  PRIVATE
    test-data/test-data.qrc
)

target_sources(test_dspsuper_global_integration_time
  PRIVATE
    test-data/test-data.qrc
)
