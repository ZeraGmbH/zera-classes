add_library(transformer1module MODULE
    ${CMAKE_CURRENT_SOURCE_DIR}/transformer1modulefactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/transformer1modulefactory.h
    )

target_link_libraries(transformer1module
    PRIVATE
    transformer1module-static
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(transformer1module
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )

install(TARGETS transformer1module
    EXPORT transformer1moduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/zera-modules
    )
