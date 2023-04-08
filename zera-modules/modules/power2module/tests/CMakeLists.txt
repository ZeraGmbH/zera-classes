# we need source files - install runs after tests
add_definitions(-DCONFIG_PATH="${CMAKE_CURRENT_SOURCE_DIR}/../configs")

function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_sources(${_testname}
            PRIVATE
            ${XML_SCHEMA_RESOURCE}
        )
        target_link_libraries(${_testname}
            PRIVATE
            power2module-static
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
    test_power2_config_load
)

