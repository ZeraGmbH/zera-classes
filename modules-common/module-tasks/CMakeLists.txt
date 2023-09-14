add_subdirectory(lib)

if(NOT SKIP_UNITTESTS)
    add_subdirectory(testlib)
    add_subdirectory(tests)
endif()
