add_library(dosagemodule MODULE
    ${XML_SCHEMA_RESOURCE_DOSAGEMODULE}
    ${CMAKE_CURRENT_SOURCE_DIR}/dosagemodulefactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dosagemodulefactory.h
    )

target_link_libraries(dosagemodule
    PRIVATE
    dosagemodule-static
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(dosagemodule
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )

install(TARGETS dosagemodule
    EXPORT dosagemoduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/zera-modules
    )

