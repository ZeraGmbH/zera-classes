file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(zera-i2c-devices-testlib STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(zera-i2c-devices-testlib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/i2c-devices>
    )

target_link_libraries(zera-i2c-devices-testlib
    PUBLIC
    zera-i2c-devices
)

set_target_properties(zera-i2c-devices-testlib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zera-i2c-devices-testlib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS zera-i2c-devices-testlib NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS zera-i2c-devices-testlib
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/i2c-devices
    )
