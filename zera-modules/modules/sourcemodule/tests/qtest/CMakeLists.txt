add_library(source-module-testhelpers STATIC
    test_globals.h
    test_globals.cpp
    sourcedeviceerrorinjection-forunittest.h
    sourcedeviceerrorinjection-forunittest.cpp
    sourcescannerwithinstancecount-forunittest.h
    sourcescannerwithinstancecount-forunittest.cpp
    veincomponentsetnotifier-forunittest.h
    veincomponentsetnotifier-forunittest.cpp
    sourcestateperiodicpollerfortest.cpp
    sourcestateperiodicpollerfortest.h
    )

target_link_libraries(source-module-testhelpers
    PUBLIC
    Qt5::Test
    sourcemodule-static
    Zera::VfStorageHash
    Zera::zera-timers-testlib
    )

target_include_directories(source-module-testhelpers
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    ${PROJECT_SOURCE_DIR}/src
    )

SETUP_QTESTS(
   test_iodevice
   test_ioqueue
   test_iotransferdata
   test_sourceio
   test_sourcescanner
   test_sourcedevicemanager
   test_sourcestatecontroller
   test_sourceperiodicpollerstate
   test_sourceswitchjson
   test_sourcedevicefacade
)
