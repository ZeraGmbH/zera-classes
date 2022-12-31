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
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/dev>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )

target_link_libraries(zera-i2c-unittest
    PUBLIC
    gtest
    gtest_main
    Qt5::Core
    zera-i2c-devices
    zerai2c
)
