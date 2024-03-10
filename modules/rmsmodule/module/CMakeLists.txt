add_library(rmsmodule MODULE
    ${CMAKE_CURRENT_SOURCE_DIR}/rmsmodulefactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/rmsmodulefactory.h
    )

target_link_libraries(rmsmodule
    PRIVATE
    rmsmodule-static
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(rmsmodule
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )

install(TARGETS rmsmodule
    EXPORT rmsmoduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/zera-modules
    )

