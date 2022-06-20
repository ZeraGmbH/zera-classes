cmake_minimum_required(VERSION 3.1)

#set project name and language
project(zera-sourcemodule LANGUAGES CXX
    VERSION ${REPO_VERSION}
)

#Automoc and RCC ON. Needed in QT projects
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
#C++ 11 Flag
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Adding cmake functions like feature_summary()
include(FeatureSummary)
include(GNUInstallDirs)
include(FindPkgConfig)

#Find dependecies
find_package(Qt5 COMPONENTS Core Gui Network CONFIG REQUIRED)
find_package(VfEvent REQUIRED)
find_package(VfHelpers REQUIRED)
find_Package(VfComponent REQUIRED)
find_Package(zera-json-params REQUIRED)
find_package(serialportasyncblock REQUIRED)
pkg_check_modules(XERCES xerces-c REQUIRED)
find_Package(VfCpp REQUIRED)

#sum up project Files 
file(GLOB SOURCES 
    ${PROJECT_SOURCE_DIR}/src/*.cpp
    ${PROJECT_SOURCE_DIR}/src/io-device/*.cpp
    ${PROJECT_SOURCE_DIR}/src/io-queue/*.cpp
    ${PROJECT_SOURCE_DIR}/src/json/*.cpp
    ${PROJECT_SOURCE_DIR}/src/message-texts/*.cpp
    ${PROJECT_SOURCE_DIR}/src/source-device/*.cpp
    ${PROJECT_SOURCE_DIR}/src/source-protocols/*.cpp
    ${PROJECT_SOURCE_DIR}/src/source-scanner/*.cpp
    ${PROJECT_SOURCE_DIR}/src/transaction-ids/*.cpp
    ${PROJECT_SOURCE_DIR}/src/vein/*.cpp
    )


file(GLOB HEADER
    ${PROJECT_SOURCE_DIR}/src/*.h
    ${PROJECT_SOURCE_DIR}/src/io-device/*.h
    ${PROJECT_SOURCE_DIR}/src/io-queue/*.h
    ${PROJECT_SOURCE_DIR}/src/json/*.h
    ${PROJECT_SOURCE_DIR}/src/message-texts/*.h
    ${PROJECT_SOURCE_DIR}/src/source-device/*.h
    ${PROJECT_SOURCE_DIR}/src/source-protocols/*.h
    ${PROJECT_SOURCE_DIR}/src/source-scanner/*.h
    ${PROJECT_SOURCE_DIR}/src/transaction-ids/*.h
    ${PROJECT_SOURCE_DIR}/src/vein/*.h
    )

file(GLOB RESOURCES 
    ${PROJECT_SOURCE_DIR}/*.qrc
    )

file(GLOB CONFIG_FILES
    ${PROJECT_SOURCE_DIR}/configs/*.xml
)

file(GLOB CONFIG_VALIDATOR
    ${PROJECT_SOURCE_DIR}/configs/*.xsd
)

file(GLOB SOURCE_DEVINFO
    ${PROJECT_SOURCE_DIR}/deviceinfo/*.json
)

# auto-generate c++ xml access from xsd
add_custom_command(
    OUTPUT
        ${PROJECT_BINARY_DIR}/sourcemodule-config.hxx
        ${PROJECT_BINARY_DIR}/sourcemodule-config.cxx
    COMMAND
        xsdcxx cxx-tree --std c++11 --generate-serialization --output-dir "${PROJECT_BINARY_DIR}" ${CONFIG_VALIDATOR}
    DEPENDS
        ${CONFIG_VALIDATOR}
)

add_custom_target(sourcemodule-config-wrappers ALL
    DEPENDS
        ${PROJECT_BINARY_DIR}/sourcemodule-config.hxx
        ${PROJECT_BINARY_DIR}/sourcemodule-config.cxx
)

set_property(
    SOURCE
        ${PROJECT_BINARY_DIR}/sourcemodule-config.hxx
        ${PROJECT_BINARY_DIR}/sourcemodule-config.cxx
    PROPERTY SKIP_AUTOMOC ON
)


# check for valid config files / display in qt-creator
add_custom_target(sourcemodule-configs ALL
    DEPENDS ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    SOURCES ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    COMMAND xmllint --noout --schema ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    )

# Note 1: In OE we do not (yet) have jsonlint-php so it must be optional
# Note 2: In case we make COMMAND in target below conditional on JSONLINT,
#         qt-creator does not display source-deviceinfo. That's what the
#         'echo' dance is for
find_program (JSONLINT NAMES jsonlint-php echo)
if(${JSONLINT} MATCHES ".*echo.*")
    message(WARNING "Program jsonlint-php not found. On Fedora install php-jsonlint")
endif()

# check for valid devinfo / display in qt-creator
add_custom_target(source-deviceinfo-dont-forget-qrc ALL
    SOURCES ${SOURCE_DEVINFO}
    COMMAND ${JSONLINT} ${SOURCE_DEVINFO}
    )

# check if all configs made it into resource.qrc
foreach(sourceconfigfile ${SOURCE_DEVINFO})
    get_filename_component(filename ${sourceconfigfile} NAME)
    file(STRINGS ${PROJECT_SOURCE_DIR}/resource.qrc lines REGEX "${filename}")
    if(NOT lines)
        message(FATAL_ERROR "${filename} was not added to resource.qrc - source will not work!!")
    endif()
endforeach()

set(COMMON_LIBS
    ${XERCES_LIBRARIES}
    zerabasemodule
    zeramisc
    serialportasyncblock
    )

add_library(sourcemodule-static STATIC
    ${SOURCES}
    ${HEADER}
    ${RESOURCES}
    )

target_link_libraries(sourcemodule-static
    PUBLIC
    ${COMMON_LIBS}
    )

target_include_directories(sourcemodule-static
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

file(GLOB MODULE_SOURCES
    ${PROJECT_SOURCE_DIR}/src/module-gluelogic/*.cpp
    )

file(GLOB MODULE_HEADERS
    ${PROJECT_SOURCE_DIR}/src/module-gluelogic/*.h
    )

add_library(sourcemodule MODULE
    ${MODULE_SOURCES}
    ${MODULE_HEADERS}
    ${PROJECT_BINARY_DIR}/sourcemodule-config.cxx
    )

target_link_libraries(sourcemodule
    PUBLIC
    sourcemodule-static
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

# we build a module -> no public headers
target_include_directories(sourcemodule
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

# install library
install(TARGETS sourcemodule
    EXPORT sourcemoduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/zera-modules
    )

# configure *ConfigVersion.cmake
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    sourcemoduleConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMajorVersion
    )

install(
    FILES
        ${CONFIG_FILES}
        ${CONFIG_VALIDATOR}
    DESTINATION
        ${SET_ZC_CONFIG_PATH}
    )

if(NOT CMAKE_CROSSCOMPILING AND ${GTest_FOUND})
    message(STATUS "unittest sourcemodule")
    add_subdirectory(tests)
    enable_testing()

    add_test(NAME    sourcemodule-unittest
             COMMAND sourcemodule-unittest)

    file(GLOB UNITTESTS
        tests/gtest/unittest-*.cpp
    )

    add_executable(sourcemodule-unittest
        ${UNITTESTS}
        ${PROJECT_BINARY_DIR}/sourcemodule-config.cxx
    )
    # constexpr / EXPECT_EQ -> linker problems for c++ < 17
    set_property(TARGET sourcemodule-unittest PROPERTY CXX_STANDARD 17)

    target_include_directories(sourcemodule-unittest
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        )

    target_link_libraries(sourcemodule-unittest
        PUBLIC
        gtest
        gtest_main
        sourcemodule-static
    )

    add_test(NAME    sourcemodule-unittest-qt
             COMMAND sourcemodule-unittest-qt)

    file(GLOB QTUNITTESTS
        tests/qtest/qt-unittest-*.cpp
        tests/qtest/qt-unittest-*.h
        tests/qtest/*forunittest.h
        tests/qtest/*forunittest.cpp
    )

    add_executable(sourcemodule-unittest-qt
        tests/qtest/main-unittest-qt.h
        tests/qtest/main-unittest-qt.cpp # must be first!!!
        ${QTUNITTESTS}
        tests/qtest/test_globals.cpp
    )

    target_include_directories(sourcemodule-unittest-qt
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        )

    find_package(VfStorageHash REQUIRED)

    target_link_libraries(sourcemodule-unittest-qt
        PUBLIC
        VeinMeta::VfStorageHash
        Qt5::Test
        sourcemodule-static
    )

endif()
