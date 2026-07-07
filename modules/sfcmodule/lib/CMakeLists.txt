file(GLOB SOURCES 
    *.cpp
    )

file(GLOB HEADER
    *.h
    )

add_library(sfcmodule-lib SHARED
    ${SOURCES}
    ${HEADER}
    )

target_include_directories(sfcmodule-lib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(sfcmodule-lib
    PUBLIC
    zera-basemodule
    modules-common-sec
    zera-meas-mode-management
    Zera::zera-math
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

set_target_properties(sfcmodule-lib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(sfcmodule-lib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS sfcmodule-lib NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS sfcmodule-lib
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
