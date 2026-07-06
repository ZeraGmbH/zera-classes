file(GLOB SOURCES 
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

add_library(dosagemodule-lib SHARED
    dosagemodulexmlschema.qrc
    ${SOURCES}
    ${HEADER}
    )

target_include_directories(dosagemodule-lib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(dosagemodule-lib
    PUBLIC
    zera-basemodule
    zera-meas-mode-management
    Zera::zera-math
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

set_target_properties(dosagemodule-lib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(dosagemodule-lib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS dosagemodule-lib NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS dosagemodule-lib
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
