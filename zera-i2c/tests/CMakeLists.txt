if(NOT CMAKE_CROSSCOMPILING AND ${GTest_FOUND})
    message(STATUS "unittest zera-i2c")
    enable_testing()

    add_test(NAME    zera-i2c-unittest
             COMMAND zera-i2c-unittest)

    file(GLOB UNITTESTS
        unittest-*.cpp
    )

    add_executable(zera-i2c-unittest
        ${UNITTESTS}
    )
    # constexpr / EXPECT_EQ -> linker problems for c++ < 17
    set_property(TARGET zera-i2c-unittest PROPERTY CXX_STANDARD 17)

    target_include_directories(zera-i2c-unittest
        PUBLIC
        $<INSTALL_INTERFACE:include/zera-classes/dev>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        )

    target_link_libraries(zera-i2c-unittest
        PUBLIC
        gtest
        gtest_main
        Qt5::Core
        zeradev
        zerai2c
    )
endif()
