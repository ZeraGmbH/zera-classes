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

find_package(Qt5 COMPONENTS Core Network Gui Test CONFIG REQUIRED)
find_package(GTest REQUIRED)
find_package(ZenuxCore REQUIRED)
find_package(ZeraMicroController REQUIRED)
find_package(VeinFramework REQUIRED)
find_package(VfLogger REQUIRED)
find_package(VfExport REQUIRED)
find_package(VfFiles REQUIRED)
find_package(zera-json-helpers REQUIRED)
find_package(SCPI REQUIRED)
find_package(serialportasyncblock REQUIRED)
find_package(ZenuxLegacyComponents REQUIRED)
find_package(ZenuxServices REQUIRED)
find_package(BlePlayground REQUIRED)

# better suggestions?
get_filename_component(SYSTEMD_UNIT_INSTALL_PATH "${CMAKE_INSTALL_PREFIX}/../lib/systemd/system" ABSOLUTE)

# As long as there is no smart way to get 'check-jsonschema' on all parts
# of our pipeline, python warnings are hacked away at users of JSONSCHEMA
find_program (JSONSCHEMA NAMES jsonschema REQUIRED)

# Note 1: In OE we do not (yet) have jsonlint-php so it must be optional
# Note 2: In case we make COMMAND in target below conditional on JSONLINT,
#         qt-creator does not display source-capabilities. That's what the
#         'echo' dance is for
find_program(JSONLINT NAMES jsonlint-php echo)
if(${JSONLINT} MATCHES ".*echo.*")
    message(WARNING "Program jsonlint-php not found. On Fedora install php-jsonlint")
endif()


set(ZC_SERVICES_IP "" CACHE STRING "Services IP address - change to machine's address for debugging")

set(OE_INSTALL_ROOT "" CACHE STRING "OE build install root - add 'add_definitions' only on test code")
set(OPERATOR_HOME "/home/operator" CACHE FILEPATH "Operator home path")
set(SET_MODMAN_SESSION_PATH "${CMAKE_INSTALL_SYSCONFDIR}/zera/modules/sessions/")
set(SET_MODMAN_CONFIG_PATH  "${CMAKE_INSTALL_SYSCONFDIR}/zera/modules/")
set(SET_MODMAN_CONTENTSET_PATH "${CMAKE_INSTALL_SYSCONFDIR}/zera/logger-contentsets/")
set(SET_MODMAN_AUTOMOUNT_PATH "/media/" CACHE STRING "Automount path")
set(SET_MODMAN_DEFAULT_SESSION "modulemanager_config.json" CACHE STRING "Default session")
set(SET_SESSION_FILES_SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/modulemanager/sessions")

add_definitions(-DMODMAN_CONFIG_PATH="${SET_MODMAN_CONFIG_PATH}")
add_definitions(-DSOURCE_STATE_PATH="${CMAKE_INSTALL_SYSCONFDIR}/zera/sourcestates")


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
            Zera::VfStorage
            Zera::VfFrameworkTest
            Zera::testlib-common
            Zera::tasks-base-testlib
            Zera::zera-timers-testlib
            Zera::service-interfaces-testlib
            )
    endforeach()
endfunction()

add_subdirectory(modulemanager)
add_subdirectory(modules)
add_subdirectory(modules-common)
add_subdirectory(modules-common-power)
add_subdirectory(modules-common-sec)
add_subdirectory(scpi-doc)
add_subdirectory(logging-snapshots)


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
