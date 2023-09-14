add_subdirectory(configs)
add_subdirectory(lib)
add_subdirectory(module)

if(NOT SKIP_UNITTESTS)
    add_subdirectory(tests)
endif()
