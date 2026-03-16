file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(dsp-support-testlib STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(dsp-support-testlib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/dsp-support-testlib>
    )

target_link_libraries(dsp-support-testlib
    PUBLIC
    dftmodule-demolib
    fftmodule-demolib
    rangemodule-demolib
    rmsmodule-demolib
)

set_target_properties(dsp-support-testlib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(dsp-support-testlib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS dsp-support-testlib NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS dsp-support-testlib
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dsp-support-testlib
    )
