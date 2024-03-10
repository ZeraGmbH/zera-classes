add_library(burden1module MODULE
    ${CMAKE_CURRENT_SOURCE_DIR}/burden1modulefactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/burden1modulefactory.h
    )

target_link_libraries(burden1module
    PRIVATE
    burden1module-static
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(burden1module
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )

install(TARGETS burden1module
    EXPORT burden1moduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/zera-modules
    )
