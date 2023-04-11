add_library(scpimodule MODULE
    ${XML_SCHEMA_RESOURCE_SCPIMODULE}
    ${CMAKE_CURRENT_SOURCE_DIR}/scpimodulefactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/scpimodulefactory.h
    )

target_link_libraries(scpimodule
    PRIVATE
    scpimodule-static
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(scpimodule
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )

install(TARGETS scpimodule
    EXPORT scpimoduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/zera-modules
    )

