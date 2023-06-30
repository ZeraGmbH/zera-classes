add_library(lambdamodule MODULE
    ${XML_SCHEMA_RESOURCE_LAMBDAMODULE}
    ${CMAKE_CURRENT_SOURCE_DIR}/lambdamodulefactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lambdamodulefactory.h
    )

target_link_libraries(lambdamodule
    PRIVATE
    lambdamodule-static
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(lambdamodule
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )

install(TARGETS lambdamodule
    EXPORT lambdamoduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/zera-modules
    )

