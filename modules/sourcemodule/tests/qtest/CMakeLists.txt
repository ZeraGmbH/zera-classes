function(SETUP_QTESTS_MODULE)
    SETUP_QTESTS(${ARGN})
    foreach(_testname ${ARGN})
        target_sources(${_testname}
            PRIVATE
        )
        target_link_libraries(${_testname}
            PRIVATE
            sourcemodule-static
            sourcemodule-testlib
            zera-modulemanager-testlib
        )
    endforeach()
endfunction()

SETUP_QTESTS_MODULE(
   test_iodevice
   test_ioqueue
   test_iodevice_zera_scpi_net
   test_iotransferdata
   test_sourceio
   test_sourcescanner
   test_sourcedevicemanager
   test_sourcestatecontroller
   test_sourceperiodicpollerstate
   test_sourceswitchjson
   test_sourcedevicefacade
   test_source_full
   test_file_dissapepear_watcher
   test_sourceswitchjsoninternal
)

target_sources(test_sourceswitchjsoninternal
   PRIVATE
     test-data/test-data.qrc
)

target_sources(test_source_full
  PRIVATE
    test-data/test-data.qrc
)
