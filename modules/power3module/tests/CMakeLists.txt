function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_sources(${_testname}
            PRIVATE
            ${XML_SCHEMA_RESOURCE_POWER3MODULE}
        )
        target_link_libraries(${_testname}
            PRIVATE
            power3module-static
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
    test_power3_config_load
)

