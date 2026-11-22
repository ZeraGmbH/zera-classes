# Dumps dummy target to see them in IDE
file(GLOB_RECURSE TESTFILES
    test-data/*
)
# Unique target name!
add_custom_target(sourcemodule-test-data ALL
    SOURCES ${TESTFILES}
    )

function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_sources(${_testname}
            PRIVATE
        )
        target_link_libraries(${_testname}
            PRIVATE
            sourcemodule-lib
            sourcemodule-testlib
            zera-modulemanager-testlib
        )
        target_compile_definitions(${_testname}
            PRIVATE
            TEST_SRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
   test_iodevice
   test_ioqueue
   test_iotransferdata
   test_sourceio
   test_sourcescanner
   test_sourcedevicemanager
   test_sourcestatecontroller
   test_sourceperiodicpollerstate
   test_sourceswitchjson
   test_sourcedeviceextserial
   test_source_full
   test_file_dissapepear_watcher
   test_sourceswitchjsoninternal
   test_sourceswitchjsonangleadjustmentdecorator
)

target_sources(test_sourceswitchjsoninternal
   PRIVATE
     test-data/test-data.qrc
)

target_sources(test_source_full
  PRIVATE
    test-data/test-data.qrc
)

target_sources(test_sourceswitchjsonangleadjustmentdecorator
   PRIVATE
     test-data/test-data.qrc
)
