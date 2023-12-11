add_definitions(-DSCPI_DOC_BUILD_PATH="${CMAKE_CURRENT_BINARY_DIR}")
add_definitions(-DSCPI_DOC_SOURCE_PATH="${CMAKE_CURRENT_SOURCE_DIR}")

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

add_executable(scpi-doc-generator
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    )

target_link_libraries(scpi-doc-generator
    PRIVATE
    zera-modulemanager-testlib
)

add_subdirectory(xml-to-html)

install(TARGETS scpi-doc-generator
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
