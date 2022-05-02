cmake_minimum_required(VERSION 3.1)

#set project name and language
project(zera-rangemodule LANGUAGES CXX
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

#Find dependecies
find_package(Qt5 COMPONENTS Core Gui Network CONFIG REQUIRED)
find_package(VfComponent REQUIRED)
find_package(VfHelpers REQUIRED)
find_package(VfEvent REQUIRED)


#sum up project Files 
file(GLOB SOURCES 
    ${PROJECT_SOURCE_DIR}/src/*.cpp
    )

file(GLOB HEADER
    ${PROJECT_SOURCE_DIR}/src/*.h
    )

file(GLOB RESOURCES 
    ${PROJECT_SOURCE_DIR}/*.qrc
    )


file(GLOB CONFIG_FILES
    ${PROJECT_SOURCE_DIR}/src/*.xml
)

file(GLOB CONFIG_VALIDATOR
    ${PROJECT_SOURCE_DIR}/src/*.xsd
)

file(GLOB TOPICS
    ${PROJECT_SOURCE_DIR}/topic/*.json
)

# Note 1: In OE we do not (yet) have jsonlint-php so it must be optional
# Note 2: In case we make COMMAND in target below conditional on JSONLINT,
#         qt-creator does not display source-deviceinfo. That's what the
#         'echo' dance is for
find_program (JSONLINT NAMES jsonlint-php echo)
if(${JSONLINT} MATCHES ".*echo.*")
    message(WARNING "Program jsonlint-php not found. On Fedora install php-jsonlint")
endif()

# check for valid topic / display in qt-creator
add_custom_target(topic-dont-forget-qrc ALL
    SOURCES ${TOPICS}
    COMMAND ${JSONLINT} ${TOPICS}
    )

# check if all configs made it into resource.qrc
foreach(topic ${TOPICS})
    get_filename_component(filename ${topic} NAME)
    file(STRINGS ${PROJECT_SOURCE_DIR}/resources.qrc lines REGEX "${filename}")
    if(NOT lines)
        message(FATAL_ERROR "${filename} was not added to resource.qrc - topic will not work!!")
    endif()
endforeach()

# check for valid config files / display in qt-creator
add_custom_target(rangemodule-configs ALL
    DEPENDS ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    SOURCES ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    COMMAND xmllint --noout --schema ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    )

# create library
add_library(rangemodule MODULE
    ${SOURCES}
    ${HEADER}
    ${RESOURCES}
    )

set(COMMON_LIBS
    Qt5::Core
    Qt5::Gui
    Qt5::Network
    VeinMeta::VfComponent
    VeinMeta::VfHelpers
    VeinMeta::VfEvent
    MeasurementModuleInterface
    zerabasemodule
    zeramisc
    zeraxmlconfig
    zeravalidator
    )

target_link_libraries(rangemodule
    PRIVATE
    ${COMMON_LIBS}
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

# we build a module -> no public headers
target_include_directories(rangemodule
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    ${CMAKE_CURRENT_SOURCE_DIR}
    )


# install library
install(TARGETS rangemodule
    EXPORT rangemoduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/zera-modules
    )

# configure *ConfigVersion.cmake
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    rangemoduleConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMinorVersion
    )


if(NOT ZC_NO_CONFIG_INSTALL)
    install(
        FILES ${CONFIG_FILES} ${CONFIG_VALIDATOR}
        DESTINATION ${SET_ZC_CONFIG_PATH}
        )
endif()


find_package(Qt5Test REQUIRED)
find_package(GTest)
if(NOT CMAKE_CROSSCOMPILING AND ${GTest_FOUND})
    message(STATUS "unittest rangemodule")
    enable_testing()

    add_test(NAME    rangemodule-unittest
             COMMAND rangemodule-unittest)

    file(GLOB UNITTESTS
        tests/unittest-*.cpp
    )

    add_executable(rangemodule-unittest
        ${SOURCES}
        ${HEADER}
        ${RESOURCES}
        ${UNITTESTS}
    )
    # constexpr / EXPECT_EQ -> linker problems for c++ < 17
    set_property(TARGET rangemodule-unittest PROPERTY CXX_STANDARD 17)

    target_include_directories(rangemodule-unittest
        PUBLIC
        $<INSTALL_INTERFACE:include/rangemodule>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        )

    target_link_libraries(rangemodule-unittest
        PUBLIC
        gtest
        gtest_main
        ${COMMON_LIBS}
    )

if(FALSE) # we have no qt-unittests yet
    add_test(NAME    rangemodule-unittest-qt
             COMMAND rangemodule-unittest-qt)

    file(GLOB QTUNITTESTS
        tests/qt-unittest-*.cpp
        tests/qt-unittest-*.h
    )

    add_executable(rangemodule-unittest-qt
        tests/main-unittest-qt.h
        tests/main-unittest-qt.cpp # must be first!!!
        ${SOURCES}
        ${HEADER}
        ${RESOURCES}
        ${QTUNITTESTS}
    )

    target_include_directories(rangemodule-unittest-qt
        PUBLIC
        $<INSTALL_INTERFACE:include/rangemodule>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        )

    target_link_libraries(rangemodule-unittest-qt
        PUBLIC
        Qt5::Test
        ${COMMON_LIBS}
    )
endif()

endif()


# spawn out some info on configuration
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)



