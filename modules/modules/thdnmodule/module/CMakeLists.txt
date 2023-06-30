add_library(thdnmodule MODULE
    ${XML_SCHEMA_RESOURCE_THDNMODULE}
    ${CMAKE_CURRENT_SOURCE_DIR}/thdnmodulefactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/thdnmodulefactory.h
    )

target_link_libraries(thdnmodule
    PRIVATE
    thdnmodule-static
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(thdnmodule
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )

install(TARGETS thdnmodule
    EXPORT thdnmoduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/zera-modules
    )

