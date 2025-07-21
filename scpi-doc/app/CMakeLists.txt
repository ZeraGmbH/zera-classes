add_definitions(-DEXPORT_DOCS_PATH="${CMAKE_CURRENT_BINARY_DIR}/")

add_executable(scpi-doc-generator
    main.cpp
    )

target_link_libraries(scpi-doc-generator
    PRIVATE
    scpi-doc-lib
)

install(TARGETS scpi-doc-generator
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
