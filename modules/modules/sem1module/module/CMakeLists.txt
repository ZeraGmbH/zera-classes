add_library(sem1module MODULE
    ${XML_SCHEMA_RESOURCE_SEM1MODULE}
    ${CMAKE_CURRENT_SOURCE_DIR}/sem1modulefactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sem1modulefactory.h
    )

target_link_libraries(sem1module
    PRIVATE
    sem1module-static
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(sem1module
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )

install(TARGETS sem1module
    EXPORT sem1moduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/zera-modules
    )
