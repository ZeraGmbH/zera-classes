# Inspired by
# https://github.com/KDAB/kdabtv/blob/master/Qt-Widgets-and-more/UnitTests/Tests/CMakeLists.txt

find_package(VfStorageHash REQUIRED)

function(SETUP_TESTS)
    foreach(_testname ${ARGN})
        add_test(NAME test_${_testname}
                 COMMAND test_${_testname})
        add_executable(test_${_testname}
            test_${_testname}.h
            test_${_testname}.cpp
            test_globals.cpp
            )
        target_include_directories(test_${_testname}
            PRIVATE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
            ${PROJECT_SOURCE_DIR}/src
            )
        target_link_libraries(test_${_testname}
            PUBLIC
            Qt${QT_MAJOR_VERISION}::Test
            VeinMeta::VfStorageHash
            sourcemodule-static
            )
    endforeach()
endfunction()

SETUP_TESTS(
   iodevice
)
