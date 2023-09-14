add_subdirectory(modules)

if(NOT SKIP_UNITTESTS)
    add_subdirectory(testlib)
endif()

