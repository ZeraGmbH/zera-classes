# Dumps dummy target to see them in IDE
file(GLOB_RECURSE TESTFILES
    test-data/*
)
# Unique target name!
add_custom_target(scpimodule-test-data ALL
    SOURCES ${TESTFILES}
    )

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
            Zera::testlib-memallocs
        )
        target_compile_definitions(${_testname}
            PRIVATE
            TEST_SRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
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
    test_scpi_mem_leaks
    test_scpi_compare_component_behavior
)
