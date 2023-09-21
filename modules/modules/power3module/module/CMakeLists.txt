add_library(power3module MODULE
    ${XML_SCHEMA_RESOURCE_POWER3MODULE}
    ${CMAKE_CURRENT_SOURCE_DIR}/power3modulefactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/power3modulefactory.h
    )

target_link_libraries(power3module
    PRIVATE
    power3module-static
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(power3module
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )

install(TARGETS power3module
    EXPORT power3moduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/zera-modules
    )
