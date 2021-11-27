cmake_minimum_required(VERSION 3.1)

#set project name and language
project(zera-dev LANGUAGES CXX
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
find_package(Qt5 COMPONENTS Core CONFIG REQUIRED)



#sum up project Files 
file(GLOB SOURCES 
    ${PROJECT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${PROJECT_SOURCE_DIR}/*.h
    )

file(GLOB RESOURCES 
    ${PROJECT_SOURCE_DIR}/*.qrc
    )

#create library 
add_library(zeradev SHARED
    ${SOURCES}
    ${HEADER}
    ${RESOURCES}
    )

set(COMMON_LIBS
    Qt5::Core
    zerai2c
    zeramath
    zeramisc
    )

target_link_libraries(zeradev
    PRIVATE
    ${COMMON_LIBS}
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )


# announce headers - target perspective
target_include_directories(zeradev
    PUBLIC
    $<INSTALL_INTERFACE:include/zera-classes/dev>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/zera-misc
    )

#set target Version
set_target_properties(zeradev PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zeradev PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

#generate export header
include(GenerateExportHeader)
generate_export_header(zeradev)

#install export header
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zeradev_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/dev
    )



# install library
install(TARGETS zeradev
    EXPORT zeradevExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

# configure *ConfigVersion.cmake
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    zeradevConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMinorVersion
    )

# configure *Config.cmake
configure_file(zeradevConfig.cmake.in zeradevConfig.cmake @ONLY)


# install *Config(Version).cmake
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/zeradevConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/zeradevConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zeradev
    )

# install targets cmake-files
install(EXPORT zeradevExport
    FILE zeradevTargets.cmake
    NAMESPACE ZeraClasses::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zeradev
    )

# install public headers
install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/dev
    )


find_package(Qt5Test REQUIRED)
find_package(GTest)
if(NOT CMAKE_CROSSCOMPILING AND ${GTest_FOUND})
    message(STATUS "unittest zera-dev")
    enable_testing()

    add_test(NAME    zera-dev-unittest
             COMMAND zera-dev-unittest)

    file(GLOB UNITTESTS
        tests/unittest-*.cpp
    )

    add_executable(zera-dev-unittest
        ${SOURCES}
        ${HEADER}
        ${RESOURCES}
        ${UNITTESTS}
    )
    # constexpr / EXPECT_EQ -> linker problems for c++ < 17
    set_property(TARGET zera-dev-unittest PROPERTY CXX_STANDARD 17)

    target_include_directories(zera-dev-unittest
        PUBLIC
        $<INSTALL_INTERFACE:include/zera-classes/dev>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/zera-misc
        )

    target_link_libraries(zera-dev-unittest
        PUBLIC
        gtest
        gtest_main
        ${COMMON_LIBS}
    )
endif()
# spawn out some info on configuration
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)



