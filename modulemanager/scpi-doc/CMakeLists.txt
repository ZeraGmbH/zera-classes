add_definitions(-DSCPI_DOC_PLAYGROUND_PATH="${CMAKE_CURRENT_BINARY_DIR}/scpi-xmls")

add_executable(scpi-doc-generator
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    )

target_link_libraries(scpi-doc-generator
    PRIVATE
    zera-modulemanager-testlib
)
