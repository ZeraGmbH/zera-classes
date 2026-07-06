file(GLOB SOURCES 
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

add_library(lambdamodule-lib SHARED
    ${SOURCES}
    ${HEADER}
    )

target_include_directories(lambdamodule-lib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(lambdamodule-lib
    PUBLIC
    zera-basemodule
    zera-meas-mode-management
    Zera::zera-math
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

set_target_properties(lambdamodule-lib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(lambdamodule-lib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS lambdamodule-lib NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS lambdamodule-lib
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
