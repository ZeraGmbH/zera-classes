add_definitions(-DSCPI_LOCAL_FILE_PATH="${OPERATOR_HOME}/scpi-files")

add_subdirectory(configs)
add_subdirectory(lib)
add_subdirectory(module)
add_subdirectory(scpi-files)
add_subdirectory(testlib)
add_subdirectory(tests)
