function(SETUP_QTESTS_COMMON_POWER)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_link_libraries(${_testname}
            PUBLIC
            zera-meas-mode-management-testlib
            )
    endforeach()
endfunction()

SETUP_QTESTS_COMMON_POWER(
    test_measmodephasesetstrategyphasesvar
    test_measmodephasesetstrategyphasesfixed
    test_measmodephasesetstrategy2wire
    test_measmodephasesetstrategy2wirefixedphase
    test_measmodephasesetstrategy4wire
    test_measmodephasesetstrategyxwire
    test_measmode
    test_measmodecatalog
    test_measmodeselector
    test_measmodebroker
    test_measmodephasepersistency
    test_phasevalidatorstringgenerator
    test_measmodeinfo
)
