cmake_minimum_required(VERSION 3.1)

project(zera-classes LANGUAGES CXX)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(PROJECT_VERSION_MAJOR "1")
set(PROJECT_VERSION_MINOR "2")
set(PROJECT_VERSION_PATCH "0")
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

find_package(Qt5 COMPONENTS Core Network CONFIG REQUIRED)
find_package(Qt5Test REQUIRED)
find_package(GTest REQUIRED)

include(FeatureSummary)
include(GNUInstallDirs)
include(GenerateExportHeader)
include(CMakePackageConfigHelpers)
include(FindPkgConfig)

option(ZC_BUILD_MODULES "Build modules for modulemanager" ON)
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
            tasks-base-testlib
            zera-timers-testlib
            zera-proxy-testlib
            service-interfaces-testlib
            module-tasks-testlib
            source-module-testhelpers
            zera-basemodule
            VeinMeta::VfStorageHash
            )
    endforeach()
endfunction()

# TBD: Own repo to get rid of ZC_BUILD_MODULES
add_subdirectory(zera-i2c)
add_subdirectory(zera-intel-hex)

if(ZC_BUILD_MODULES)
    find_package(Qt5 COMPONENTS XmlPatterns CONFIG REQUIRED)
    find_package(VfEvent REQUIRED)
    find_package(VfHelpers REQUIRED)
    find_package(VfComponent REQUIRED)
    find_package(VfStorageHash REQUIRED)
    find_Package(VfCpp REQUIRED)
    find_package(zera-json-params REQUIRED)
    find_package(xiqnet REQUIRED)
    find_package(SCPI REQUIRED)
    find_package(serialportasyncblock REQUIRED)

    pkg_check_modules(XERCES xerces-c REQUIRED)
    set(SET_ZC_CONFIG_PATH "${CMAKE_INSTALL_SYSCONFDIR}/zera/modules/")
    add_definitions(-DZC_CONFIG_PATH="${SET_ZC_CONFIG_PATH}")

    add_subdirectory(zera-math)
    add_subdirectory(zera-modules)
    add_subdirectory(zera-proxy)
    add_subdirectory(zera-modules-common)
    add_subdirectory(zera-tasks-base)
    add_subdirectory(zera-timers)
    add_subdirectory(zera-xmlconfig)

endif(ZC_BUILD_MODULES)

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
