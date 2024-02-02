add_library(rangemodule MODULE
    ${XML_SCHEMA_RESOURCE_RANGEMODULE}
    ${CMAKE_CURRENT_SOURCE_DIR}/rangemodulefactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/rangemodulefactory.h
    )

target_link_libraries(rangemodule
    PRIVATE
    rangemodule-static
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(rangemodule
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )

install(TARGETS rangemodule
    EXPORT rangemoduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/zera-modules
    )
