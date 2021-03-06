cmake_minimum_required(VERSION 3.1)

#set project name and language
project(zera-basemodules LANGUAGES CXX
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
find_package(VfEvent REQUIRED)
find_package(VfHelpers REQUIRED)
find_package(VfComponent REQUIRED)


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
add_library(zerabasemodule SHARED
    ${SOURCES}
    ${HEADER}
    ${RESOURCES}
    )



target_link_libraries(zerabasemodule 
    PRIVATE
    Qt5::Gui
    Qt5::Network
    zeravalidator
    zeraxmlconfig
    VeinMeta::VfEvent
    VeinMeta::VfHelpers
    VeinMeta::VfComponent
    )


# announce headers - target perspective
target_include_directories(zerabasemodule
    PUBLIC
    $<INSTALL_INTERFACE:include/zera-classes/basemodule>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

#set target Version
set_target_properties(zerabasemodule PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zerabasemodule PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

#generate export header
include(GenerateExportHeader)
generate_export_header(zerabasemodule)

#install export header
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zerabasemodule_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/basemodule
    )



# install library
install(TARGETS zerabasemodule
    EXPORT zerabasemoduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

# configure *ConfigVersion.cmake
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    zerabasemoduleConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMinorVersion
    )

# configure *Config.cmake
configure_file(zerabasemoduleConfig.cmake.in zerabasemoduleConfig.cmake @ONLY)

message(STATUS "BINDIR: " ${CMAKE_CURRENT_BINARY_DIR})

# install *Config(Version).cmake
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/zerabasemoduleConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/zerabasemoduleConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zerabasemodule
    )

# install targets cmake-files
install(EXPORT zerabasemoduleExport
    FILE zerabasemoduleTargets.cmake
    NAMESPACE ZeraClasses::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zerabasemodule
    )

# install public headers
install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/basemodule
    )

# spawn out some info on configuration
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)



