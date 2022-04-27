file(GLOB SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    )

add_library(zeraxmlconfig SHARED
    ${SOURCES}
    ${HEADER}
    )

target_link_libraries(zeraxmlconfig 
    PRIVATE
    Qt5::Core
    Qt5::XmlPatterns
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

# announce headers - target perspective
target_include_directories(zeraxmlconfig
    PUBLIC
    $<INSTALL_INTERFACE:include/zeraxmlconfig>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

set_target_properties(zeraxmlconfig PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zeraxmlconfig PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(zeraxmlconfig)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zeraxmlconfig_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zeraxmlconfig
    )

install(TARGETS zeraxmlconfig
    EXPORT zeraxmlconfigExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

# configure *ConfigVersion.cmake
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    zeraxmlconfigConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMinorVersion
    )

# configure *Config.cmake
configure_file(zeraxmlconfigConfig.cmake.in zeraxmlconfigConfig.cmake @ONLY)

# install *Config(Version).cmake
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/zeraxmlconfigConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/zeraxmlconfigConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zeraxmlconfig
    )

# install targets cmake-files
install(EXPORT zeraxmlconfigExport
    FILE zeraxmlconfigTargets.cmake
    NAMESPACE ZeraClasses::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zeraxmlconfig
    )

# install public headers
install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zeraxmlconfig
    )

if(NOT CMAKE_CROSSCOMPILING AND ${GTest_FOUND})
    message(STATUS "unittest zeraxmlconfig")
    enable_testing()

    add_test(NAME    zeraxmlconfig-unittest
             COMMAND zeraxmlconfig-unittest)

    file(GLOB UNITTESTS
        tests/gtest/unittest-*.cpp
    )

    add_executable(zeraxmlconfig-unittest
        ${UNITTESTS}
    )
    # constexpr / EXPECT_EQ -> linker problems for c++ < 17
    set_property(TARGET zeraxmlconfig-unittest PROPERTY CXX_STANDARD 17)

    target_include_directories(zeraxmlconfig-unittest
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        )

    target_link_libraries(zeraxmlconfig-unittest
        PUBLIC
        gtest
        gtest_main
        zeraxmlconfig
    )

    add_test(NAME    zeraxmlconfig-unittest-qt
             COMMAND zeraxmlconfig-unittest-qt)

    file(GLOB QTUNITTESTS
        tests/qtest/qt-unittest-*.cpp
        tests/qtest/qt-unittest-*.h
        tests/qtest/*forunittest.h
        tests/qtest/*forunittest.cpp
    )

    add_executable(zeraxmlconfig-unittest-qt
        tests/qtest/main-unittest-qt.h
        tests/qtest/main-unittest-qt.cpp # must be first!!!
        ${QTUNITTESTS}
    )

    target_include_directories(zeraxmlconfig-unittest-qt
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        )

    target_link_libraries(zeraxmlconfig-unittest-qt
        PUBLIC
        Qt5::Test
        zeraxmlconfig
    )

endif()
