add_library(samplemodule MODULE
    ${XML_SCHEMA_RESOURCE_SAMPLEMODULE}
    ${CMAKE_CURRENT_SOURCE_DIR}/samplemodulefactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/samplemodulefactory.h
    )

target_link_libraries(samplemodule
    PRIVATE
    samplemodule-static
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(samplemodule
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )

install(TARGETS samplemodule
    EXPORT samplemoduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/zera-modules
    )

