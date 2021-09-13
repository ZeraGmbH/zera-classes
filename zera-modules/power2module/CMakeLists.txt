cmake_minimum_required(VERSION 3.1)

#set project name and language
project(zera-power2module LANGUAGES CXX
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
    ${PROJECT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${PROJECT_SOURCE_DIR}/src/*.h
     ${PROJECT_SOURCE_DIR}/*.h
    )

file(GLOB RESOURCES 
    ${PROJECT_SOURCE_DIR}/*.qrc
    )

file(GLOB CONFIG_FILES
    ${PROJECT_SOURCE_DIR}/src/*.xml
)

file(GLOB CONFIG_VALIDATOR
    ${PROJECT_SOURCE_DIR}/src/*.xsd
)

# check for valid config files / display in qt-creator
add_custom_target(power2module-configs ALL
    DEPENDS ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    SOURCES ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    COMMAND xmllint --noout --schema ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    )

# create library
add_library(power2module MODULE
    ${SOURCES}
    ${HEADER}
    ${RESOURCES}
    )

target_link_libraries(power2module 
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
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

# announce headers - target perspective
target_include_directories(power2module
    PUBLIC
    $<INSTALL_INTERFACE:include/power2module>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

# install library
install(TARGETS power2module
    EXPORT power2moduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/zera-modules
    )

# configure *ConfigVersion.cmake
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    power2moduleConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMinorVersion
    )

if(NOT ZC_NO_CONFIG_INSTALL)
    install(
        FILES ${CONFIG_FILES} ${CONFIG_VALIDATOR}
        DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/zera/modules
        )
endif()

# spawn out some info on configuration
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
