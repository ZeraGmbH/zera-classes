add_library(power1module MODULE
    ${XML_SCHEMA_RESOURCE_POWER1MODULE}
    ${CMAKE_CURRENT_SOURCE_DIR}/power1modulefactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/power1modulefactory.h
    )

target_link_libraries(power1module
    PRIVATE
    power1module-static
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(power1module
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )

install(TARGETS power1module
    EXPORT power1moduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/zera-modules
    )
