add_library(adjustmentmodule MODULE
    ${CMAKE_CURRENT_SOURCE_DIR}/adjustmentmodulefactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/adjustmentmodulefactory.h
    )

target_link_libraries(adjustmentmodule
    PRIVATE
    adjustmentmodule-static
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(adjustmentmodule
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )

install(TARGETS adjustmentmodule
    EXPORT adjustmentmoduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/zera-modules
    )
