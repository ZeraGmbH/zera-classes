file(GLOB SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
     ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

file(GLOB RESOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.qrc
    )

add_library(transformer1module MODULE
    ${SOURCES}
    ${HEADER}
    ${RESOURCES}
    )

target_link_libraries(transformer1module 
    PRIVATE
    zera-basemodule
    Zera::zera-math
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(transformer1module
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

install(TARGETS transformer1module
    EXPORT transformer1moduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/zera-modules
    )

add_subdirectory(configs)
