add_definitions(-DSCPI_DOC_SOURCE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/xml-to-html")

add_subdirectory(app)
add_subdirectory(lib)
add_subdirectory(xml-to-html)

