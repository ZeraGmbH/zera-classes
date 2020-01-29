cmake_minimum_required(VERSION 3.1)

#set project name and language
project(zera-i2c LANGUAGES CXX
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
find_package(Qt5 COMPONENTS Gui Network CONFIG REQUIRED)


#Sum up sources
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
add_library(zerai2c SHARED
    ${SOURCES}
    ${HEADER}
    ${RESOURCES}
    )



target_link_libraries(zerai2c 
    PRIVATE
    Qt5::Core
    )


# announce headers - target perspective
target_include_directories(zerai2c
    PUBLIC
    $<INSTALL_INTERFACE:include/zera-comm/zerai2c>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

#set target Version
set_target_properties(zerai2c PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zerai2c PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})


#generate export header
include(GenerateExportHeader)
generate_export_header(zerai2c)

#install export header
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zerai2c_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zerai2c
    )



# install library
install(TARGETS zerai2c
    EXPORT zerai2cExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

# configure *ConfigVersion.cmake
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    zerai2cConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMinorVersion
    )

# configure *Config.cmake
configure_file(zerai2cConfig.cmake.in zerai2cConfig.cmake @ONLY)


# install *Config(Version).cmake
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/zerai2cConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/zerai2cConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zerai2c
    )

# install targets cmake-files
install(EXPORT zerai2cExport
    FILE zerai2cTargets.cmake
    NAMESPACE ZeraClasses::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zerai2c
    )

# install public headers
install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/i2c
    )

# spawn out some info on configuration
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)



