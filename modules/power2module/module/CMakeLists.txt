add_library(power2module MODULE
    ${CMAKE_CURRENT_SOURCE_DIR}/power2modulefactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/power2modulefactory.h
    )

target_link_libraries(power2module
    PRIVATE
    power2module-static
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(power2module
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )

install(TARGETS power2module
    EXPORT power2moduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/zera-modules
    )
