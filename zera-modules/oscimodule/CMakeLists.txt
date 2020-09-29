cmake_minimum_required(VERSION 3.1)

#set project name and language
project(zera-oscimodule LANGUAGES CXX
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
find_Package(VfComponent REQUIRED)


#sum up project Files 
file(GLOB SOURCES 
    ${PROJECT_SOURCE_DIR}/src/*.cpp
    )

file(GLOB HEADER
    ${PROJECT_SOURCE_DIR}/src/*.h
    )

file(GLOB RESOURCES 
    ${PROJECT_SOURCE_DIR}/*.qrc
    )

if(NOT ZC_NO_CONFIG_INSTALL)
    file(GLOB OTHER_FILES
        ${PROJECT_SOURCE_DIR}/src/*.xml
        ${PROJECT_SOURCE_DIR}/src/*.xsd
    )
endif()

#create library 
add_library(oscimodule MODULE
    ${SOURCES}
    ${HEADER}
    ${RESOURCES}
    )



target_link_libraries(oscimodule 
    PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Network
    VeinMeta::VfEvent
    VeinMeta::VfHelpers
    VeinMeta::VfComponent
    MeasurementModuleInterface
    zerabasemodule
    zeraproxy
    zeramisc
    zeraxmlconfig
    zeravalidator 
    )


# announce headers - target perspective
target_include_directories(oscimodule
    PUBLIC
    $<INSTALL_INTERFACE:include/oscimodule>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    )


# install library
install(TARGETS oscimodule
    EXPORT oscimoduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/zera-modules
    )

# configure *ConfigVersion.cmake
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    oscimoduleConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMinorVersion
    )

install(
    FILES ${OTHER_FILES}
    DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/zera/modules
    )

# spawn out some info on configuration
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)



