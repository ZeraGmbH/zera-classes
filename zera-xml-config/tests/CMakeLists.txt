if(NOT CMAKE_CROSSCOMPILING AND ${GTest_FOUND})
    message(STATUS "unittest zeraxmlconfig")
    enable_testing()

    add_test(NAME    zeraxmlconfig-unittest
             COMMAND zeraxmlconfig-unittest)

    file(GLOB UNITTESTS
        gtest/unittest-*.cpp
    )

    add_executable(zeraxmlconfig-unittest
        ${UNITTESTS}
    )
    # constexpr / EXPECT_EQ -> linker problems for c++ < 17
    set_property(TARGET zeraxmlconfig-unittest PROPERTY CXX_STANDARD 17)

    target_include_directories(zeraxmlconfig-unittest
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        )

    target_link_libraries(zeraxmlconfig-unittest
        PUBLIC
        gtest
        gtest_main
        zeraxmlconfig
    )

    add_test(NAME    zeraxmlconfig-unittest-qt
             COMMAND zeraxmlconfig-unittest-qt)

    set(SET_SOURCEPATH "${CMAKE_CURRENT_SOURCE_DIR}")
    add_definitions(-DSOURCEPATH="${SET_SOURCEPATH}")

    file(GLOB QTUNITTESTS
        qtest/qt-unittest-*.cpp
        qtest/qt-unittest-*.h
        qtest/*forunittest.h
        qtest/*forunittest.cpp
    )

    add_executable(zeraxmlconfig-unittest-qt
        qtest/main-unittest-qt.h
        qtest/main-unittest-qt.cpp # must be first!!!
        ${QTUNITTESTS}
        test-data.qrc
    )

    target_include_directories(zeraxmlconfig-unittest-qt
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        )

    target_link_libraries(zeraxmlconfig-unittest-qt
        PUBLIC
        Qt5::Test
        zeraxmlconfig
    )

endif()
