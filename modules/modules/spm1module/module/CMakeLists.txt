add_library(spm1module MODULE
    ${XML_SCHEMA_RESOURCE_SPM1MODULE}
    ${CMAKE_CURRENT_SOURCE_DIR}/spm1modulefactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/spm1modulefactory.h
    )

target_link_libraries(spm1module
    PRIVATE
    spm1module-static
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(spm1module
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )

install(TARGETS spm1module
    EXPORT spm1moduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/zera-modules
    )
