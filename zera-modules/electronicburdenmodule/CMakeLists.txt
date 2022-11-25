cmake_minimum_required(VERSION 3.1)

#set project name and language
project(zera-electronicburdenmodule LANGUAGES CXX
    VERSION ${REPO_VERSION}
)

#Automoc and RCC ON. Needed in QT projects
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

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
    ${PROJECT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${PROJECT_SOURCE_DIR}/src/*.h
     ${PROJECT_SOURCE_DIR}/*.h
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
        ${PROJECT_BINARY_DIR}/electronicburdenmodule-config.hxx
        ${PROJECT_BINARY_DIR}/electronicburdenmodule-config.cxx
    COMMAND
        xsdcxx cxx-tree --std c++11 --generate-serialization --output-dir "${PROJECT_BINARY_DIR}" ${CONFIG_VALIDATOR}
    DEPENDS
        ${CONFIG_VALIDATOR}
)

add_custom_target(electronicburdenmodule-config-wrappers ALL
    DEPENDS
        ${PROJECT_BINARY_DIR}/electronicburdenmodule-config.hxx
        ${PROJECT_BINARY_DIR}/electronicburdenmodule-config.cxx
)

set_property(
    SOURCE
        ${PROJECT_BINARY_DIR}/electronicburdenmodule-config.hxx
        ${PROJECT_BINARY_DIR}/electronicburdenmodule-config.cxx
    PROPERTY SKIP_AUTOMOC ON
)


# check for valid config files / display in qt-creator
add_custom_target(electronicburdenmodule-configs ALL
    DEPENDS ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    SOURCES ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    COMMAND xmllint --noout --schema ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    )

# Note 1: In OE we do not (yet) have jsonlint-php so it must be optional
# Note 2: In case we make COMMAND in target below conditional on JSONLINT,
#         qt-creator does not display electronicburden-deviceinfo. That's what the
#         'echo' dance is for
find_program (JSONLINT NAMES jsonlint-php echo)
if(${JSONLINT} MATCHES ".*echo.*")
    message(WARNING "Program jsonlint-php not found. On Fedora install php-jsonlint")
endif()

# check for valid devinfo / display in qt-creator
add_custom_target(electronicburden-deviceinfo-dont-forget-qrc ALL
    SOURCES ${SOURCE_DEVINFO}
    COMMAND ${JSONLINT} ${SOURCE_DEVINFO}
    )

# check if all configs made it into resource.qrc
foreach(electronicburdenconfigfile ${SOURCE_DEVINFO})
    get_filename_component(filename ${electronicburdenconfigfile} NAME)
    file(STRINGS ${PROJECT_SOURCE_DIR}/resource.qrc lines REGEX "${filename}")
    if(NOT lines)
        message(FATAL_ERROR "${filename} was not added to resource.qrc - electronic burden will not work!!")
    endif()
endforeach()

set(COMMON_LIBS
    ${XERCES_LIBRARIES}
    zerabasemodule
    zeramisc
    serialportasyncblock
    )

# create library
add_library(electronicburdenmodule MODULE
    ${SOURCES}
    ${HEADER}
    ${RESOURCES}
    ${PROJECT_BINARY_DIR}/electronicburdenmodule-config.cxx
    )

target_link_libraries(electronicburdenmodule
    PUBLIC
    ${COMMON_LIBS}
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(electronicburdenmodule
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/electronicburdenmodule>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

# we build a module -> no public headers
target_include_directories(electronicburdenmodule
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

# install library
install(TARGETS electronicburdenmodule
    EXPORT electronicburdenmoduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/zera-modules
    )

# configure *ConfigVersion.cmake
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    electronicburdenmoduleConfigVersion.cmake
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

find_package(Qt5Test REQUIRED)
find_package(GTest)
if (false)  # todo: unit tests!
#if(NOT CMAKE_CROSSCOMPILING AND ${GTest_FOUND})
    message(STATUS "unittest electronicburdenmodule")
    enable_testing()

    add_test(NAME    electronicburdenmodule-unittest
             COMMAND electronicburdenmodule-unittest)

    file(GLOB UNITTESTS
        tests/unittest-*.cpp
    )

    add_executable(electronicburdenmodule-unittest
        ${SOURCES}
        ${HEADER}
        ${RESOURCES}
        ${UNITTESTS}
        ${PROJECT_BINARY_DIR}/electronicburdenmodule-config.cxx
    )
    # constexpr / EXPECT_EQ -> linker problems for c++ < 17
    set_property(TARGET electronicburdenmodule-unittest PROPERTY CXX_STANDARD 17)

    target_include_directories(electronicburdenmodule-unittest
        PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/electronicburdenmodule>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        )

    target_link_libraries(electronicburdenmodule-unittest
        PUBLIC
        gtest
        gtest_main
        ${COMMON_LIBS}
    )

    add_test(NAME    electronicburdenmodule-unittest-qt
             COMMAND electronicburdenmodule-unittest-qt)

    file(GLOB QTUNITTESTS
        tests/qt-unittest-*.cpp
        tests/qt-unittest-*.h
    )

    add_executable(electronicburdenmodule-unittest-qt
        tests/main-unittest-qt.h
        tests/main-unittest-qt.cpp # must be first!!!
        ${SOURCES}
        ${HEADER}
        ${RESOURCES}
        ${QTUNITTESTS}
        ${PROJECT_BINARY_DIR}/electronicburdenmodule-config.cxx
    )

    target_include_directories(electronicburdenmodule-unittest-qt
        PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/electronicburdenmodule>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        )

    target_link_libraries(electronicburdenmodule-unittest-qt
        PUBLIC
        Qt5::Test
        ${COMMON_LIBS}
    )

endif()


# spawn out some info on configuration
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
