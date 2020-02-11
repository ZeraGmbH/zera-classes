cmake_minimum_required(VERSION 3.1)

#set project name and language
project(zera-MeasurementModuleInterface LANGUAGES CXX
    VERSION ${REPO_VERSION}
)

#Automoc and RCC ON. Needed in QT projects
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
#C++ 11 Flag
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Set project Version
set(MeasurementModuleInterface_VERSION_MAJOR "1")
set(MeasurementModuleInterface_VERSION_MINOR "0")
set(MeasurementModuleInterface_VERSION_PATCH "8")

set(PROJECT_VERSION "${MeasurementModuleInterface_VERSION_MAJOR}.${MeasurementModuleInterface_VERSION_MINOR}.${MeasurementModuleInterface_VERSION_PATCH}")

#Adding cmake functions like feature_summary()
include(FeatureSummary)
include(GNUInstallDirs)

#Find dependecies
find_package(Qt5 COMPONENTS Gui CONFIG REQUIRED)



#sum up project Files 
file(GLOB SOURCES 
    ${PROJECT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${PROJECT_SOURCE_DIR}/*.h
    )

#create library 
add_library(MeasurementModuleInterface SHARED
    ${HEADER}
    )



target_link_libraries(MeasurementModuleInterface 
    PRIVATE
    Qt5::Gui
    )


# announce headers - target perspective
target_include_directories(MeasurementModuleInterface
    PUBLIC
    $<INSTALL_INTERFACE:include/zera-modules>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

#set target Version
set_target_properties(MeasurementModuleInterface PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(MeasurementModuleInterface PROPERTIES SOVERSION ${MeasurementModuleInterface_VERSION_MAJOR})



# install library
install(TARGETS MeasurementModuleInterface
    EXPORT MeasurementModuleInterfaceExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

# configure *ConfigVersion.cmake
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    MeasurementModuleInterfaceConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMinorVersion
    )

# configure *Config.cmake
configure_file(MeasurementModuleInterfaceConfig.cmake.in MeasurementModuleInterfaceConfig.cmake @ONLY)

message(STATUS "BINDIR: " ${CMAKE_CURRENT_BINARY_DIR})

# install *Config(Version).cmake
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/MeasurementModuleInterfaceConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/MeasurementModuleInterfaceConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MeasurementModuleInterface
    )

# install targets cmake-files
install(EXPORT MeasurementModuleInterfaceExport
    FILE MeasurementModuleInterfaceTargets.cmake
    NAMESPACE ZeraClasses::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MeasurementModuleInterface
    )

# install public headers
install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-modules
    )

# spawn out some info on configuration
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)



