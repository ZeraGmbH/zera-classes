add_library(modemodule MODULE
    ${XML_SCHEMA_RESOURCE_MODEMODULE}
    ${CMAKE_CURRENT_SOURCE_DIR}/modemodulefactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modemodulefactory.h
    )

target_link_libraries(modemodule
    PRIVATE
    modemodule-static
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(modemodule
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )

install(TARGETS modemodule
    EXPORT modemoduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/zera-modules
    )
