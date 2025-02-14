function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_link_libraries(${_testname}
            PRIVATE
            fftmodule-static
            zera-modulemanager-testlib
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
    test_fft_config_load
    test_fft_module_regression
)

target_sources(test_fft_module_regression
  PRIVATE
    test-data/test-data.qrc
)
