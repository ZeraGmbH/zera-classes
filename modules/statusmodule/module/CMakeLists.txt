add_library(statusmodule MODULE
    ${XML_SCHEMA_RESOURCE_STATUSMODULE}
    ${CMAKE_CURRENT_SOURCE_DIR}/statusmodulefactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/statusmodulefactory.h
    )

target_link_libraries(statusmodule
    PRIVATE
    statusmodule-static
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(statusmodule
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )

install(TARGETS statusmodule
    EXPORT statusmoduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/zera-modules
    )
