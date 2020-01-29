cmake_minimum_required(VERSION 3.1)

#set project name and language
project(zera-math LANGUAGES CXX
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
add_library(zeramath SHARED
    ${SOURCES}
    ${HEADER}
    ${RESOURCES}
    )



target_link_libraries(zeramath 
    PRIVATE
    Qt5::Core
    )


# announce headers - target perspective
target_include_directories(zeramath
    PUBLIC
    $<INSTALL_INTERFACE:include/zeramath>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

#set target Version
set_target_properties(zeramath PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zeramath PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

#generate export header
include(GenerateExportHeader)
generate_export_header(zeramath)

#install export header
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zeramath_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zeramath
    )



# install library
install(TARGETS zeramath
    EXPORT zeramathExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

# configure *ConfigVersion.cmake
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    zeramathConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMinorVersion
    )

# configure *Config.cmake
configure_file(zeramathConfig.cmake.in zeramathConfig.cmake @ONLY)

# install *Config(Version).cmake
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/zeramathConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/zeramathConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zeramath
    )

# install targets cmake-files
install(EXPORT zeramathExport
    FILE zeramathTargets.cmake
    NAMESPACE ZeraClasses
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zeramath
    )

# install public headers
install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/math
    )

# spawn out some info on configuration
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)



