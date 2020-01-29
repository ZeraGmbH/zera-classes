cmake_minimum_required(VERSION 3.1)

#set project name and language
project(zera-validator LANGUAGES CXX
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
find_package(Qt5 COMPONENTS Gui CONFIG REQUIRED)
find_package(VfHelpers REQUIRED)
find_package(VfComponent REQUIRED)
find_package(VfEvent REQUIRED)



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
add_library(zeravalidator SHARED
    ${SOURCES}
    ${HEADER}
    ${RESOURCES}
    )



target_link_libraries(zeravalidator 
    PRIVATE
    Qt5::Gui
    VeinMeta::VfEvent
    VeinMeta::VfComponent
    VeinMeta::VfHelpers
    )


# announce headers - target perspective
target_include_directories(zeravalidator
    PUBLIC
    $<INSTALL_INTERFACE:include/zeravalidator>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

#set target Version
set_target_properties(zeravalidator PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zeravalidator PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

#generate export header
include(GenerateExportHeader)
generate_export_header(zeravalidator)

#install export header
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zeravalidator_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zeravalidator
    )



# install library
install(TARGETS zeravalidator
    EXPORT zeravalidatorExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

# configure *ConfigVersion.cmake
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    zeravalidatorConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMinorVersion
    )

# configure *Config.cmake
configure_file(zeravalidatorConfig.cmake.in zeravalidatorConfig.cmake @ONLY)


# install *Config(Version).cmake
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/zeravalidatorConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/zeravalidatorConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zeravalidator
    )

# install targets cmake-files
install(EXPORT zeravalidatorExport
    FILE zeravalidatorTargets.cmake
    NAMESPACE ZeraClasses
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zeravalidator
    )

# install public headers
install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zeravalidator
    )

# spawn out some info on configuration
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)



