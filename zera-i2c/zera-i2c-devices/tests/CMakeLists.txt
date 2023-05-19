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


# Inspired by
# https://github.com/KDAB/kdabtv/blob/master/Qt-Widgets-and-more/UnitTests/Tests/CMakeLists.txt
function(SETUP_I2C_QTESTS)
    foreach(_testname ${ARGN})
        add_test(NAME ${_testname}
                 COMMAND ${_testname})
        add_executable(${_testname}
            ${_testname}.h
            ${_testname}.cpp
            )
        target_link_libraries(${_testname}
            PUBLIC
            Qt5::Test
            Zera::tasks-base-testlib
            Zera::zera-timers-testlib
            zera-i2c-devices-testlib
            )
    endforeach()
endfunction()

SETUP_I2C_QTESTS(
    test_booloaderstopper
)
