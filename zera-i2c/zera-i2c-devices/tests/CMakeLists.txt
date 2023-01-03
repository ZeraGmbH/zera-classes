add_test(NAME    zera-i2c-devices-tests
         COMMAND zera-i2c-devices-tests)

file(GLOB UNITTESTS
    unittest-*.cpp
)

add_executable(zera-i2c-devices-tests
    ${UNITTESTS}
)
# constexpr / EXPECT_EQ -> linker problems for c++ < 17
set_property(TARGET zera-i2c-devices-tests PROPERTY CXX_STANDARD 17)

target_link_libraries(zera-i2c-devices-tests
    PUBLIC
    gtest
    gtest_main
    Qt5::Core
    zera-i2c-devices
)
