file(GLOB SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(zera-xmlconfig SHARED
    ${SOURCES}
    ${HEADER}
    )

target_link_libraries(zera-xmlconfig
    PUBLIC
    Qt5::Core
    Qt5::XmlPatterns
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(zera-xmlconfig
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-xmlconfig>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

set_target_properties(zera-xmlconfig PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zera-xmlconfig PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(zera-xmlconfig)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zera-xmlconfig_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-xmlconfig
    )

export(TARGETS zera-xmlconfig FILE ZeraClassesTargets.cmake)
install(TARGETS zera-xmlconfig
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-xmlconfig
    )

