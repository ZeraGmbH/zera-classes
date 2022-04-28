file(GLOB SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(zeradev SHARED
    ${SOURCES}
    ${HEADER}
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

set_target_properties(zeradev PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zeradev PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(zeradev)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zeradev_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/dev
    )

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

if(NOT CMAKE_CROSSCOMPILING AND ${GTest_FOUND})
    message(STATUS "unittest zera-i2c")
    enable_testing()

    add_test(NAME    zera-i2c-unittest
             COMMAND zera-i2c-unittest)

    file(GLOB UNITTESTS
        tests/unittest-*.cpp
    )

    add_executable(zera-i2c-unittest
        ${SOURCES}
        ${HEADER}
        ${UNITTESTS}
    )
    # constexpr / EXPECT_EQ -> linker problems for c++ < 17
    set_property(TARGET zera-i2c-unittest PROPERTY CXX_STANDARD 17)

    target_include_directories(zera-i2c-unittest
        PUBLIC
        $<INSTALL_INTERFACE:include/zera-classes/dev>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/zera-misc
        )

    target_link_libraries(zera-i2c-unittest
        PUBLIC
        gtest
        gtest_main
        ${COMMON_LIBS}
    )
endif()
