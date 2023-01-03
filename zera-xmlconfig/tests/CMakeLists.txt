add_test(NAME    zera-xmlconfig-unittest-qt
         COMMAND zera-xmlconfig-unittest-qt)

set(SET_SOURCEPATH "${CMAKE_CURRENT_SOURCE_DIR}")
add_definitions(-DSOURCEPATH="${SET_SOURCEPATH}")

file(GLOB QTUNITTESTS
    qtest/qt-unittest-*.cpp
    qtest/qt-unittest-*.h
    qtest/*forunittest.h
    qtest/*forunittest.cpp
)

add_executable(zera-xmlconfig-unittest-qt
    qtest/main-unittest-qt.h
    qtest/main-unittest-qt.cpp # must be first!!!
    ${QTUNITTESTS}
    test-data.qrc
)

target_link_libraries(zera-xmlconfig-unittest-qt
    PUBLIC
    Qt5::Test
    zera-xmlconfig
)
