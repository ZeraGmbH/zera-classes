cmake_minimum_required(VERSION 3.1)

#set project name and language
project(zera-proxy LANGUAGES CXX
    VERSION ${REPO_VERSION}
)

#Automoc and RCC ON. Needed in QT projects
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
#C++ 11 Flag
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


option(useSystemd "Using libsystemd" ON)

#Adding cmake functions like feature_summary()
include(FeatureSummary)
include(GNUInstallDirs)

#Find dependecies
find_package(Qt5 COMPONENTS Gui Network CONFIG REQUIRED)
find_package(xiqnet REQUIRED)


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
add_library(zeraproxy SHARED
    ${SOURCES}
    ${HEADER}
    ${RESOURCES}
    )



target_link_libraries(zeraproxy 
    PUBLIC
    Qt5::Network
    VeinMeta::xiqnet
    )


# announce headers - target perspective
target_include_directories(zeraproxy
    PUBLIC
    $<INSTALL_INTERFACE:include/zera-classes/proxy>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

#set target Version
set_target_properties(zeraproxy PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zeraproxy PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

#generate export header
include(GenerateExportHeader)
generate_export_header(zeraproxy)

#install export header
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zeraproxy_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/proxy
    )



# install library
install(TARGETS zeraproxy
    EXPORT zeraproxyExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

# configure *ConfigVersion.cmake
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    zeraproxyConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMinorVersion
    )

# configure *Config.cmake
configure_file(zeraproxyConfig.cmake.in zeraproxyConfig.cmake @ONLY)

message(STATUS "BINDIR: " ${CMAKE_CURRENT_BINARY_DIR})

# install *Config(Version).cmake
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/zeraproxyConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/zeraproxyConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zeraproxy
    )

# install targets cmake-files
install(EXPORT zeraproxyExport
    FILE zeraproxyTargets.cmake
    NAMESPACE ZeraClasses::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zeraproxy
    )

# install public headers
install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/proxy
    )

# spawn out some info on configuration
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)



