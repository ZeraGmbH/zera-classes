add_library(sfcmodule MODULE
    ${XML_SCHEMA_RESOURCE_SFCMODULE}
    ${CMAKE_CURRENT_SOURCE_DIR}/sfcmodulefactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sfcmodulefactory.h
    )

target_link_libraries(sfcmodule
    PRIVATE
    sfcmodule-static
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(sfcmodule
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )

install(TARGETS sfcmodule
    EXPORT sfcmoduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/zera-modules
    )

