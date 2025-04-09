function(SETUP_QTESTS_MODULE_TASKS)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_link_libraries(${_testname}
            PUBLIC
            module-tasks
            Zera::scpi-testlib
            )
    endforeach()
endfunction()

add_subdirectory(pcb-server)
