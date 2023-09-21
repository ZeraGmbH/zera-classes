add_library(sourcemodule MODULE
    ${XML_SCHEMA_RESOURCE_SOURCEMODULE}
    ${CMAKE_CURRENT_SOURCE_DIR}/sourcemodulefactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sourcemodulefactory.h
    )

target_link_libraries(sourcemodule
    PUBLIC
    sourcemodule-static
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(sourcemodule
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )

install(TARGETS sourcemodule
    EXPORT sourcemoduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/zera-modules
    )

