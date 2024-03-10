add_library(fftmodule MODULE
    ${CMAKE_CURRENT_SOURCE_DIR}/fftmodulefactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/fftmodulefactory.h
    )

target_link_libraries(fftmodule
    PRIVATE
    fftmodule-static
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(fftmodule
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )

install(TARGETS fftmodule
    EXPORT fftmoduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/zera-modules
    )

