add_library(dspsupermodule MODULE
    ${CMAKE_CURRENT_SOURCE_DIR}/dspsupermodulefactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dspsupermodulefactory.h
    )

target_link_libraries(dspsupermodule
    PRIVATE
    dspsupermodule-static
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(dspsupermodule
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )

install(TARGETS dspsupermodule
    EXPORT dspsupermoduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/zera-modules
    )

