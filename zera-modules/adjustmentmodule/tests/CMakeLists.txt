# Inspired by
# https://github.com/KDAB/kdabtv/blob/master/Qt-Widgets-and-more/UnitTests/Tests/CMakeLists.txt

find_package(VfStorageHash REQUIRED)
set(SET_MODMAN_CONFIG_PATH  "${CMAKE_INSTALL_SYSCONFDIR}/zera/modules/")
add_definitions(-DMODMAN_CONFIG_PATH="${SET_MODMAN_CONFIG_PATH}")

add_library(testhelper STATIC
    )

target_include_directories(testhelper
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

target_link_libraries(testhelper
    PUBLIC
    Qt5::Core
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
        target_include_directories(${_testname}
            PRIVATE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
            ${PROJECT_SOURCE_DIR}/src
            )
        target_link_libraries(${_testname}
            PUBLIC
            Qt5::Test
            adjustmentmodule-static
            zerabasemodule
            testhelper
            VeinMeta::VfStorageHash
           )
    endforeach()
endfunction()

SETUP_TESTS(
    test_adjustmentmoduleactivator

)
