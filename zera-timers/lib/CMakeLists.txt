file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )
file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

add_library(zera-timers SHARED
    ${HEADER}
    ${SOURCES}
    )

target_link_libraries(zera-timers
    PUBLIC
    Qt5::Core
    )

target_include_directories(zera-timers
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/timers>
    )

set_target_properties(zera-timers PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zera-timers PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(zera-i2c)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zera-i2c_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/timers
    )

export(TARGETS zera-timers FILE ZeraClassesTargets.cmake)
install(TARGETS zera-timers
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/timers
    )

