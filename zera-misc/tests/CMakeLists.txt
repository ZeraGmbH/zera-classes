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


# Inspired by
# https://github.com/KDAB/kdabtv/blob/master/Qt-Widgets-and-more/UnitTests/Tests/CMakeLists.txt

add_library(helpers STATIC
    taskfortest.h
    taskfortest.cpp
    )

target_include_directories(helpers
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(helpers
    PUBLIC
    Qt5::Core
    Qt5::Test
    zeramisc
    )

enable_testing()

function(SETUP_TESTS)
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
            helpers
            )
    endforeach()
endfunction()

SETUP_TESTS(
    test_taskfortest
    test_tasksequence
    test_timeoutdecorator
    test_taskparallel
    test_taskextraerrorhandler
)
