add_library(blemodule MODULE
    ${XML_SCHEMA_RESOURCE_BLEMODULE}
    ${CMAKE_CURRENT_SOURCE_DIR}/blemodulefactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/blemodulefactory.h
    )

target_link_libraries(blemodule
    PRIVATE
    blemodule-static
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(blemodule
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )

install(TARGETS blemodule
    EXPORT blemoduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/zera-modules
    )

