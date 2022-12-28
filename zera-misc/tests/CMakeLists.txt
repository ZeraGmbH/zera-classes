enable_testing()

add_test(NAME    zera-misc-unittest
         COMMAND zera-misc-unittest)

file(GLOB UNITTESTS
    unittest-*.cpp
)

add_executable(zera-misc-unittest
    ${UNITTESTS}
)
# constexpr / EXPECT_EQ -> linker problems for c++ < 17
set_property(TARGET zera-misc-unittest PROPERTY CXX_STANDARD 17)

target_include_directories(zera-misc-unittest
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/misc>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )

target_link_libraries(zera-misc-unittest
    PUBLIC
    gtest
    gtest_main
    Qt5::Core
    zeramisc
)
