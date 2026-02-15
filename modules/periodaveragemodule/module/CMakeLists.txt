add_library(periodaveragemodule MODULE
    ${CMAKE_CURRENT_SOURCE_DIR}/periodaveragemodulefactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/periodaveragemodulefactory.h
    )

target_link_libraries(periodaveragemodule
    PRIVATE
    periodaveragemodule-static
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(periodaveragemodule
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )

install(TARGETS periodaveragemodule
    EXPORT periodaveragemoduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/zera-modules
    )

