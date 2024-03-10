add_library(referencemodule MODULE
    ${CMAKE_CURRENT_SOURCE_DIR}/referencemodulefactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/referencemodulefactory.h
    )

target_link_libraries(referencemodule
    PRIVATE
    referencemodule-static
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(referencemodule
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )

install(TARGETS referencemodule
    EXPORT referencemoduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/zera-modules
    )

