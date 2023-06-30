add_library(dftmodule MODULE
    ${XML_SCHEMA_RESOURCE_DFTMODULE}
    ${CMAKE_CURRENT_SOURCE_DIR}/dftmodulefactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dftmodulefactory.h
    )

target_link_libraries(dftmodule
    PRIVATE
    dftmodule-static
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(dftmodule
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )

install(TARGETS dftmodule
    EXPORT dftmoduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/zera-modules
    )

