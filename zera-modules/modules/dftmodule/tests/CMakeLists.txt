function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_sources(${_testname}
            PRIVATE
            ${XML_SCHEMA_RESOURCE_DFTMODULE}
        )
        target_link_libraries(${_testname}
            PRIVATE
            dftmodule-static
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
    test_dft_config_load
)

