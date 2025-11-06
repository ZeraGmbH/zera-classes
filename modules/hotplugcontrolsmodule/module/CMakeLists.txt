add_library(hotplugcontrolsmodule MODULE
    ${XML_SCHEMA_RESOURCE_HOTPLUGSCONTROLMODULE}
    ${CMAKE_CURRENT_SOURCE_DIR}/hotplugcontrolsmodulefactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hotplugcontrolsmodulefactory.h
    )

target_link_libraries(hotplugcontrolsmodule
    PRIVATE
    hotplugcontrolsmodule-static
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(hotplugcontrolsmodule
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )

install(TARGETS hotplugcontrolsmodule
    EXPORT hotplugcontrolsmoduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/zera-modules
)
