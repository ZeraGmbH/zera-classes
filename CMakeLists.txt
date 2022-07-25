cmake_minimum_required(VERSION 3.1)

project(zera-classes LANGUAGES CXX)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_VERSION_MAJOR "1")
set(PROJECT_VERSION_MINOR "2")
set(PROJECT_VERSION_PATCH "0")
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
set(REPO_VERSION ${PROJECT_VERSION})

find_package(Qt5 COMPONENTS Core Network CONFIG REQUIRED)
find_package(Qt5Test REQUIRED)
find_package(GTest)

include(FeatureSummary)
include(GNUInstallDirs)
include(GenerateExportHeader)
include(CMakePackageConfigHelpers)

option(ZC_BUILD_MODULES "Build modules for modulemanager" ON)
option(ZC_NO_CONFIG_INSTALL "Do not install configuration files - for debugging on PC" OFF)

set(SET_ZC_DEV_STATE_PATH "${CMAKE_INSTALL_SYSCONFDIR}/zera/states")
add_definitions(-DZC_DEV_STATE_PATH="${SET_ZC_DEV_STATE_PATH}")

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
            testhelpers
            )
    endforeach()
endfunction()

if(ZC_BUILD_MODULES)
    find_package(Qt5 COMPONENTS Gui XmlPatterns CONFIG REQUIRED)
    find_package(VfEvent REQUIRED)
    find_package(VfHelpers REQUIRED)
    find_package(VfComponent REQUIRED)
    find_package(zera-json-params REQUIRED)
    find_Package(VfCpp REQUIRED)
    find_package(xiqnet REQUIRED)
    set(SET_ZC_CONFIG_PATH "${CMAKE_INSTALL_SYSCONFDIR}/zera/modules/")
    add_definitions(-DZC_CONFIG_PATH="${SET_ZC_CONFIG_PATH}")
    add_subdirectory(zera-modules-common)
    add_subdirectory(zera-xml-config)
    add_subdirectory(zera-modules)
    add_subdirectory(zera-proxy)
endif(ZC_BUILD_MODULES)

add_subdirectory(zera-math)
add_subdirectory(zera-intel-hex-checker)
add_subdirectory(zera-mcontroller-io)
add_subdirectory(zera-i2c)
add_subdirectory(zera-misc)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
