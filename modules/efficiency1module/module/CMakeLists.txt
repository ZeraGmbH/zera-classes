add_library(efficiency1module MODULE
    ${CMAKE_CURRENT_SOURCE_DIR}/efficiency1modulefactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/efficiency1modulefactory.h
    )

target_link_libraries(efficiency1module
    PRIVATE
    efficiency1module-static
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(efficiency1module
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )

install(TARGETS efficiency1module
    EXPORT efficiency1moduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/zera-modules
    )
