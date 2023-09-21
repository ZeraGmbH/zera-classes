add_library(sourcemodule-testlib STATIC
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

target_link_libraries(sourcemodule-testlib
    PUBLIC
    Qt5::Test
    sourcemodule-static
    Zera::VfStorageHash
    Zera::zera-timers-testlib
    )

target_include_directories(sourcemodule-testlib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )
