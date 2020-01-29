cmake_minimum_required(VERSION 3.1)

#set project name and language
project(zera-xml-config LANGUAGES CXX
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
find_package(Qt5 COMPONENTS Core XmlPatterns CONFIG REQUIRED)




#sum up project Files 
file(GLOB SOURCES 
    ${PROJECT_SOURCE_DIR}/src/*.cpp
    )

file(GLOB HEADER
    ${PROJECT_SOURCE_DIR}/src/*.h
    )

file(GLOB RESOURCES 
    ${PROJECT_SOURCE_DIR}/src/*.qrc
    )

#create library 
add_library(zeraxmlconfig SHARED
    ${SOURCES}
    ${HEADER}
    ${RESOURCES}
    )



target_link_libraries(zeraxmlconfig 
    PRIVATE
    Qt5::Core
    Qt5::XmlPatterns
    )


# announce headers - target perspective
target_include_directories(zeraxmlconfig
    PUBLIC
    $<INSTALL_INTERFACE:include/zeraxmlconfig>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

#set target Version
set_target_properties(zeraxmlconfig PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zeraxmlconfig PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})


#generate export header
include(GenerateExportHeader)
generate_export_header(zeraxmlconfig)

#install export header
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zeraxmlconfig_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zeraxmlconfig
    )



# install library
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
    NAMESPACE ZeraClasses
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zeraxmlconfig
    )

# install public headers
install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zeraxmlconfig
    )

# spawn out some info on configuration
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)



