# Inspired by
# https://github.com/KDAB/kdabtv/blob/master/Qt-Widgets-and-more/UnitTests/Tests/CMakeLists.txt

find_package(VfStorageHash REQUIRED)

add_library(testhelpers STATIC
    test_globals.cpp
    sourcescannerwithinstancecount-forunittest.cpp
    )

target_link_libraries(testhelpers
    PUBLIC
    Qt5::Test
    sourcemodule-static
    VeinMeta::VfStorageHash
    )

target_include_directories(testhelpers
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    ${PROJECT_SOURCE_DIR}/src
    )

function(SETUP_TESTS)
    foreach(_testname ${ARGN})
        add_test(NAME ${_testname}
                 COMMAND ${_testname})
        add_executable(${_testname}
            ${_testname}.h
            ${_testname}.cpp
            )
        target_include_directories(${_testname}
            PRIVATE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
            ${PROJECT_SOURCE_DIR}/src
            )
        target_link_libraries(${_testname}
            PUBLIC
            testhelpers
            )
    endforeach()
endfunction()

SETUP_TESTS(
   test_iodevice
   test_ioqueue
   test_iotransferdata
   test_sourceio
)
