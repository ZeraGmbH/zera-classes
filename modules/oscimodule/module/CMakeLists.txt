add_library(oscimodule MODULE
    ${CMAKE_CURRENT_SOURCE_DIR}/oscimodulefactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/oscimodulefactory.h
    )

target_link_libraries(oscimodule
    PRIVATE
    oscimodule-static
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(oscimodule
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )

install(TARGETS oscimodule
    EXPORT oscimoduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/zera-modules
    )

