# Once we have a common testlib this can go
function(SETUP_QTESTS_RESOURCE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_link_libraries(${_testname}
            PRIVATE
            zera-resources
        )
    endforeach()
endfunction()

SETUP_QTESTS_RESOURCE(
    test_resourcebookkeeper
)
