add_library(recordermodule MODULE
    ${XML_SCHEMA_RESOURCE_HOTPLUGSCONTROLMODULE}
    ${CMAKE_CURRENT_SOURCE_DIR}/recordermodulefactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/recordermodulefactory.h
    recordermodulefactory.h recordermodulefactory.cpp
    )

target_link_libraries(recordermodule
    PRIVATE
    recordermodule-static
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(recordermodule
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )

install(TARGETS recordermodule
    EXPORT recordermoduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/zera-modules
)
