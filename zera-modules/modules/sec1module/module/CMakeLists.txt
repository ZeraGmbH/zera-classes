add_library(sec1module MODULE
    ${XML_SCHEMA_RESOURCE_SEC1MODULE}
    ${CMAKE_CURRENT_SOURCE_DIR}/sec1modulefactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sec1modulefactory.h
    )

target_link_libraries(sec1module
    PRIVATE
    sec1module-static
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(sec1module
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )

install(TARGETS sec1module
    EXPORT sec1moduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/zera-modules
    )
