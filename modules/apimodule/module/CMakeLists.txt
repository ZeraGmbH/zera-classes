add_library(apimodule MODULE
    ${XML_SCHEMA_RESOURCE_APIMODULE}
    ${CMAKE_CURRENT_SOURCE_DIR}/apimodulefactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/apimodulefactory.h
    )

target_link_libraries(apimodule
    PRIVATE
    apimodule-static
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(apimodule
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )

install(TARGETS apimodule
    EXPORT apimoduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/zera-modules
    )

