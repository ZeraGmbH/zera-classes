file(GLOB SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    )

file(GLOB RESOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.qrc
    )

add_library(burden1module MODULE
    ${SOURCES}
    ${HEADER}
    ${RESOURCES}
    )

target_link_libraries(burden1module 
    PRIVATE
    zera-basemodule
    Zera::zera-math
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(burden1module
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

install(TARGETS burden1module
    EXPORT burden1moduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/zera-modules
    )

add_subdirectory(configs)
