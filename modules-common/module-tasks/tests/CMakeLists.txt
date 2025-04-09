function(SETUP_QTESTS_MODULE_TASKS)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_link_libraries(${_testname}
            PUBLIC
            module-tasks-testlib
            )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE_TASKS(
    test_taskservertransactiontemplate
)

add_subdirectory(pcb-server)

