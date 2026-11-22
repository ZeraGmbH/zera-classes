# Dumps dummy target to see them in IDE
file(GLOB_RECURSE TESTFILES
    test-data/*
)
# Unique target name!
add_custom_target(recordermodule-test-data ALL
    SOURCES ${TESTFILES}
    )

function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_link_libraries(${_testname}
            PRIVATE
            recordermodule-lib
            zera-modulemanager-testlib
            VeinMeta::vf-logger-testlib
        )
        target_compile_definitions(${_testname}
            PRIVATE
            TEST_SRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
    test_recorder
    test_recorder_scpi
    test_recorder_reducer
    test_snapshot_recorder
)

target_sources(test_recorder
    PRIVATE
    test-data/test-data.qrc
)

target_sources(test_recorder_scpi
    PRIVATE
    test-data/test-data.qrc
)

target_sources(test_recorder_reducer
    PRIVATE
    test-data/test-data.qrc
)

target_sources(test_snapshot_recorder
    PRIVATE
    test-data/test-data.qrc
)
