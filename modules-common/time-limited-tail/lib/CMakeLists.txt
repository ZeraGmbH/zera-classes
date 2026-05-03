file(GLOB SOURCES
    *.cpp
)

file(GLOB HEADER
    *.h
)

find_package(Qt5 REQUIRED COMPONENTS Core)

add_library(time-limited-tail SHARED
    ${SOURCES}
    ${HEADER}
    )

target_link_libraries(time-limited-tail
    PUBLIC
    ${QTX}::Core
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )
target_link_libraries(time-limited-tail PRIVATE Qt5::Core)

target_include_directories(time-limited-tail
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/time-limited-tail>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

set_target_properties(time-limited-tail PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(time-limited-tail PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(time-limited-tail)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/time-limited-tail_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/time-limited-tail
    )

export(TARGETS time-limited-tail NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS time-limited-tail
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/time-limited-tail
    )
