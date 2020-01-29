cmake_minimum_required(VERSION 3.1)

#set project name and language
project(zera-misc LANGUAGES CXX
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
find_package(zeraprotobuf REQUIRED)




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
add_library(zeramisc SHARED
    ${SOURCES}
    ${HEADER}
    ${RESOURCES}
    )



target_link_libraries(zeramisc 
    PRIVATE
    Qt5::Gui
    Qt5::Network
    zeraproxy
    VeinMeta::zeraprotobuf
    )


# announce headers - target perspective
target_include_directories(zeramisc
    PUBLIC
    $<INSTALL_INTERFACE:include/zeramisc>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

#set target Version
set_target_properties(zeramisc PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zeramisc PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})


#generate export header
include(GenerateExportHeader)
generate_export_header(zeramisc)

#install export header
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zeramisc_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zeramisc
    )



# install library
install(TARGETS zeramisc
    EXPORT zeramiscExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

# configure *ConfigVersion.cmake
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    zeramiscConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMinorVersion
    )

# configure *Config.cmake
configure_file(zeramiscConfig.cmake.in zeramiscConfig.cmake @ONLY)

# install *Config(Version).cmake
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/zeramiscConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/zeramiscConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zeramisc
    )

# install targets cmake-files
install(EXPORT zeramiscExport
    FILE zeramiscTargets.cmake
    NAMESPACE ZeraClasses
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zeramisc
    )

# install public headers
install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/misc
    )

# spawn out some info on configuration
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)



