# Dumps dummy target to see them in IDE
file(GLOB_RECURSE TESTFILES
    test-data/*
)
# Unique target name!
add_custom_target(oscimodule-test-data ALL
    SOURCES ${TESTFILES}
    )

function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_link_libraries(${_testname}
            PRIVATE
            zera-modulemanager-testlib
            oscimodule-lib
        )
        target_compile_definitions(${_testname}
            PRIVATE
            TEST_SRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
    test_osci_module_regression
)

target_sources(test_osci_module_regression
  PRIVATE
    test-data/test-data.qrc
)
