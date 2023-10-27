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

find_package(Qt5 COMPONENTS Core Network Qml Concurrent Test CONFIG REQUIRED)
find_package(GTest REQUIRED)
find_package(ZenuxCore REQUIRED)
find_package(ZeraMicroController REQUIRED)
find_package(VeinFramework REQUIRED)
find_package(VfScript REQUIRED)
find_package(VfLogger REQUIRED)
find_package(VfExport REQUIRED)
find_package(VfFiles REQUIRED)
find_package(zera-json-params REQUIRED)
find_package(SCPI REQUIRED)
find_package(serialportasyncblock REQUIRED)
find_package(ZenuxLegacyComponents REQUIRED)

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
set(SET_MODMAN_TEST_SESSION "modulemanager_config_dev.json")
set(SET_SESSION_FILES_SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/modulemanager/sessions")

add_definitions(-DOPERATOR_HOME="${OPERATOR_HOME}")
add_definitions(-DMODMAN_SESSION_PATH="${SET_MODMAN_SESSION_PATH}")
add_definitions(-DMODMAN_DEFAULT_SESSION="${SET_MODMAN_DEFAULT_SESSION}")
add_definitions(-DMODMAN_TEST_SESSION="${SET_MODMAN_TEST_SESSION}")
add_definitions(-DMODMAN_CONFIG_PATH="${SET_MODMAN_CONFIG_PATH}")
add_definitions(-DMODMAN_MODULE_PATH="${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/zera-modules/")
add_definitions(-DMODMAN_CUSTOMERDATA_PATH="${OPERATOR_HOME}/customerdata/")
add_definitions(-DMODMAN_CONTENTSET_PATH="${SET_MODMAN_CONTENTSET_PATH}")
add_definitions(-DMODMAN_AUTOMOUNT_PATH="${SET_MODMAN_AUTOMOUNT_PATH}")
add_definitions(-DMODMAN_LOGGER_LOCAL_PATH="${OPERATOR_HOME}/logger")
add_definitions(-DZC_DEV_STATE_PATH="${CMAKE_INSTALL_SYSCONFDIR}/zera/states")
add_definitions(-DSESSION_FILES_SOURCE_PATH="${SET_SESSION_FILES_SOURCE_PATH}")
file(GLOB SESSION_FILES ${SET_SESSION_FILES_SOURCE_PATH}/*.json)
list(JOIN SESSION_FILES "," C_SESSION_FILES)
add_definitions(-DSESSION_FILES="${C_SESSION_FILES}")


macro(SET_MODULE_ENV MODULE_NAME)
    string(TOUPPER ${MODULE_NAME} MODULE_NAME_UPPER)
    set(MODULE_CONFIG_PATH "${CMAKE_CURRENT_SOURCE_DIR}/modules/modules/${MODULE_NAME}/configs")
    set(MODULE_PLUGIN_BUILD_PATH "${MODULE_PLUGIN_BUILD_PATH} ${CMAKE_CURRENT_BINARY_DIR}/modules/modules/${MODULE_NAME}/module")

    # Pass source paths to configs for tests. This is module-global to allow e.g
    # scpimodule running tests with other modules
    add_compile_definitions(CONFIG_SOURCES_${MODULE_NAME_UPPER}="${MODULE_CONFIG_PATH}")

    # xml-schema resources are required by modules and tests. If we put resource into
    # our library both would need some call of Q_INIT_RESOURCE. So publish resource
    # location and compile in where needed.
    set(XML_SCHEMA_RESOURCE_${MODULE_NAME_UPPER} ${MODULE_CONFIG_PATH}/xml-schema.qrc)
endmacro()

# Notes
# * SET_MODULE_ENV !MUST! be set before add_subdirectory
# * Modules not added here don't support common module folder layout (yet)
SET_MODULE_ENV(adjustmentmodule)
SET_MODULE_ENV(burden1module)
SET_MODULE_ENV(dftmodule)
SET_MODULE_ENV(efficiency1module)
SET_MODULE_ENV(fftmodule)
SET_MODULE_ENV(lambdamodule)
SET_MODULE_ENV(modemodule)
SET_MODULE_ENV(oscimodule)
SET_MODULE_ENV(power1module)
SET_MODULE_ENV(power2module)
SET_MODULE_ENV(power3module)
SET_MODULE_ENV(rangemodule)
SET_MODULE_ENV(referencemodule)
SET_MODULE_ENV(rmsmodule)
SET_MODULE_ENV(samplemodule)
SET_MODULE_ENV(scpimodule)
SET_MODULE_ENV(sec1module)
SET_MODULE_ENV(sem1module)
SET_MODULE_ENV(sourcemodule)
SET_MODULE_ENV(spm1module)
SET_MODULE_ENV(statusmodule)
SET_MODULE_ENV(thdnmodule)
SET_MODULE_ENV(transformer1module)

add_compile_definitions(MODULE_PLUGIN_BUILD_PATH="${MODULE_PLUGIN_BUILD_PATH}")

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
            zera-meas-mode-management-testlib
            Zera::VfStorageHash
            )
    endforeach()
endfunction()

add_subdirectory(modulemanager)
add_subdirectory(modules)
add_subdirectory(modules-common)
add_subdirectory(modules-common-power)
add_subdirectory(modules-common-sec)
add_subdirectory(modules-shared-data)

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
