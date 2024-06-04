file(GLOB RESOURCES 
    ${XML_SCHEMA_RESOURCES_ALL}
    ${CMAKE_CURRENT_SOURCE_DIR}/*.qrc
    )

add_executable(zera-modulemanager
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${RESOURCES}
    )

target_link_libraries(zera-modulemanager
    PRIVATE
    Qt5::Gui
    zera-modulemanager-lib
    system-metrics
)

install(TARGETS zera-modulemanager
    EXPORT zera-modulemanagerExport
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

