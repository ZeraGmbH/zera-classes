cmake_minimum_required(VERSION 3.1)

project(zera-classes LANGUAGES CXX)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(PROJECT_VERSION_MAJOR "1")
set(PROJECT_VERSION_MINOR "2")
set(PROJECT_VERSION_PATCH "0")
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

include(FeatureSummary)
include(GNUInstallDirs)
include(GenerateExportHeader)
include(CMakePackageConfigHelpers)
include(FindPkgConfig)

find_package(Qt5 COMPONENTS Core Network Qml Concurrent CONFIG REQUIRED)
find_package(Qt5Test REQUIRED)
find_package(ZenuxCore REQUIRED)
find_package(ZeraMicroController REQUIRED)
find_package(GTest REQUIRED)
find_package(Qt5 COMPONENTS XmlPatterns CONFIG REQUIRED)
find_package(VfEvent REQUIRED)
find_package(VfHelpers REQUIRED)
find_package(VfComponent REQUIRED)
find_package(VfStorageHash REQUIRED)
find_Package(VfCpp REQUIRED)
find_package(VfTcp REQUIRED)
find_package(VfNet2 REQUIRED)
find_package(VfScript REQUIRED)
find_package(VfLogger REQUIRED)
find_package(VfCryptoBridge REQUIRED)
find_package(VfExport REQUIRED)
find_package(VfFiles REQUIRED)
find_package(zera-json-params REQUIRED)
find_package(xiqnet REQUIRED)
find_package(SCPI REQUIRED)
find_package(serialportasyncblock REQUIRED)

pkg_check_modules(XERCES xerces-c REQUIRED)

set(SET_ZC_CONFIG_PATH "${CMAKE_INSTALL_SYSCONFDIR}/zera/modules/")
add_definitions(-DZC_CONFIG_PATH="${SET_ZC_CONFIG_PATH}")

set(ZC_SERVICES_IP "" CACHE STRING "Services IP address - change to machine's address for debugging")

set(SET_ZC_DEV_STATE_PATH "${CMAKE_INSTALL_SYSCONFDIR}/zera/states")
add_definitions(-DZC_DEV_STATE_PATH="${SET_ZC_DEV_STATE_PATH}")

enable_testing()
# Inspired by
# https://github.com/KDAB/kdabtv/blob/master/Qt-Widgets-and-more/UnitTests/Tests/CMakeLists.txt
function(SETUP_QTESTS)
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
            Zera::zera-i2c-devices-testlib
            service-interfaces-testlib
            module-tasks-testlib
            source-module-testhelpers
            zera-meas-mode-management-testlib
            VeinMeta::VfStorageHash
            )
    endforeach()
endfunction()

add_subdirectory(zera-math)
add_subdirectory(zera-modules)
add_subdirectory(zera-modules-common)
add_subdirectory(zera-modules-shared-data)
add_subdirectory(zera-xmlconfig)
add_subdirectory(zera-modulemanager)

write_basic_package_version_file(
    ZeraClassesConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMajorVersion
    )

configure_file(ZeraClassesConfig.cmake.in ZeraClassesConfig.cmake @ONLY)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/ZeraClassesConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/ZeraClassesConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ZeraClasses
    )

install(EXPORT ZeraClassesExport
    FILE ZeraClassesTargets.cmake
    NAMESPACE Zera::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ZeraClasses
    )

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

add_subdirectory(cmake-post-install-force-last)
