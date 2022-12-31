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

target_link_libraries(zeraxmlconfig-unittest-qt
    PUBLIC
    Qt5::Test
    zeraxmlconfig
)
