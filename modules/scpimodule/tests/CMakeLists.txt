function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_sources(${_testname}
            PRIVATE
            test-data/test-data.qrc
        )
        target_link_libraries(${_testname}
            PRIVATE
            statusmodule-lib
            rangemodule-lib
            scpimodule-testlib
            zera-modulemanager-testlib
            Zera::xml-compare-testlib
            Zera::zera-timers-testlib
            Zera::VfConvenientCodeTest
        )
        target_compile_definitions(${_testname}
            PRIVATE
            SCPI_FILE_EXAMPLE_SOURCE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/../scpi-files/scpi-file-example.txt"
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
    test_scpi_cmds_in_session
    test_scpi_xsession_change
    test_scpi_response_sorter
    test_scpi_module_sorted_responses
    test_scpi_all_responses
    test_scpi_error_and_status
    test_scpi_learn_measure_objects_and_relations
    test_scpi_vein_query_sort
    test_scpi_local_file
    test_scpi_local_file_read
)

# the test reads the shipped example file directly from the source tree (rcc does not allow ../ paths in qrc files)
