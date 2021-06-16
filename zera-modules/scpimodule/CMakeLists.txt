cmake_minimum_required(VERSION 3.1)

#set project name and language
project(zera-scpimodule LANGUAGES CXX
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
find_package(Qt5 COMPONENTS Core Gui Network SerialPort CONFIG REQUIRED)
find_package(VfEvent REQUIRED)
find_package(VfHelpers REQUIRED)
find_Package(VfComponent REQUIRED)
find_package(SCPI REQUIRED)

#sum up project Files 
file(GLOB SOURCES 
    #${PROJECT_SOURCE_DIR}/src/*.cpp
    ${PROJECT_SOURCE_DIR}/src/scpimodulefactory.cpp
    ${PROJECT_SOURCE_DIR}/src/scpimoduleconfiguration.cpp
    ${PROJECT_SOURCE_DIR}/src/scpimodule.cpp
    ${PROJECT_SOURCE_DIR}/src/scpidelegate.cpp
    ${PROJECT_SOURCE_DIR}/src/scpiserver.cpp
    ${PROJECT_SOURCE_DIR}/src/scpiinterface.cpp
    ${PROJECT_SOURCE_DIR}/src/scpimeasuredelegate.cpp
    ${PROJECT_SOURCE_DIR}/src/scpimeasure.cpp
    ${PROJECT_SOURCE_DIR}/src/scpiclient.cpp
    ${PROJECT_SOURCE_DIR}/src/scpiparameterdelegate.cpp
    ${PROJECT_SOURCE_DIR}/src/scpiinterfacedelegate.cpp
    ${PROJECT_SOURCE_DIR}/src/scpistatus.cpp
    ${PROJECT_SOURCE_DIR}/src/ieee488-2.cpp
    ${PROJECT_SOURCE_DIR}/src/baseinterface.cpp
    ${PROJECT_SOURCE_DIR}/src/moduleinterface.cpp
    ${PROJECT_SOURCE_DIR}/src/interfaceinterface.cpp
    ${PROJECT_SOURCE_DIR}/src/scpistatusdelegate.cpp
    ${PROJECT_SOURCE_DIR}/src/statusinterface.cpp
    ${PROJECT_SOURCE_DIR}/src/ieee4882interface.cpp
    ${PROJECT_SOURCE_DIR}/src/signalconnectiondelegate.cpp
    ${PROJECT_SOURCE_DIR}/src/scpiethclient.cpp
    ${PROJECT_SOURCE_DIR}/src/scpiserialclient.cpp
    ${PROJECT_SOURCE_DIR}/src/scpieventsystem.cpp
    ${PROJECT_SOURCE_DIR}/src/scpipropertydelegate.cpp
    ${PROJECT_SOURCE_DIR}/src/scpiclientinfo.cpp
    ${PROJECT_SOURCE_DIR}/src/scpicmdinfo.cpp
    )

file(GLOB HEADER
    #${PROJECT_SOURCE_DIR}/src/*.h
    ${PROJECT_SOURCE_DIR}/src/scpimodulefactory.h
    ${PROJECT_SOURCE_DIR}/src/scpimodule.h
    ${PROJECT_SOURCE_DIR}/src/scpimoduleconfiguration.h
    ${PROJECT_SOURCE_DIR}/src/scpimoduleconfigdata.h
    ${PROJECT_SOURCE_DIR}/src/debug.h
    ${PROJECT_SOURCE_DIR}/src/scpidelegate.h
    ${PROJECT_SOURCE_DIR}/src/scpiserver.h
    ${PROJECT_SOURCE_DIR}/src/scpiinterface.h
    ${PROJECT_SOURCE_DIR}/src/scpicmdinfo.h
    ${PROJECT_SOURCE_DIR}/src/scpimeasuredelegate.h
    ${PROJECT_SOURCE_DIR}/src/scpimeasure.h
    ${PROJECT_SOURCE_DIR}/src/scpiclient.h
    ${PROJECT_SOURCE_DIR}/src/scpistatus.h
    ${PROJECT_SOURCE_DIR}/src/scpiparameterdelegate.h
    ${PROJECT_SOURCE_DIR}/src/scpiinterfacedelegate.h
    ${PROJECT_SOURCE_DIR}/src/ieee488-2.h
    ${PROJECT_SOURCE_DIR}/src/baseinterface.h
    ${PROJECT_SOURCE_DIR}/src/moduleinterface.h
    ${PROJECT_SOURCE_DIR}/src/interfaceinterface.h
    ${PROJECT_SOURCE_DIR}/src/scpistatusdelegate.h
    ${PROJECT_SOURCE_DIR}/src/statusinterface.h
    ${PROJECT_SOURCE_DIR}/src/ieee4882interface.h
    ${PROJECT_SOURCE_DIR}/src/signalconnectiondelegate.h
    ${PROJECT_SOURCE_DIR}/src/scpiethclient.h
    ${PROJECT_SOURCE_DIR}/src/scpiserialclient.h
    ${PROJECT_SOURCE_DIR}/src/statusbitdescriptor.h
    ${PROJECT_SOURCE_DIR}/src/scpieventsystem.h
    ${PROJECT_SOURCE_DIR}/src/scpipropertydelegate.h
    ${PROJECT_SOURCE_DIR}/src/scpiclientinfo.h
    ${PROJECT_SOURCE_DIR}/src/scpiethclient.h
    ${PROJECT_SOURCE_DIR}/src/scpiserialclient.h
    ${PROJECT_SOURCE_DIR}/src/statusbitdescriptor.h
    ${PROJECT_SOURCE_DIR}/src/scpieventsystem.h
    ${PROJECT_SOURCE_DIR}/src/scpipropertydelegate.h
    ${PROJECT_SOURCE_DIR}/src/scpiclientinfo.h
    ${PROJECT_SOURCE_DIR}/src/scpiclientinfo.h
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
add_custom_target(scpimodule-configs ALL
    DEPENDS ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    SOURCES ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    COMMAND xmllint --noout --schema ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    )

# create library
add_library(scpimodule MODULE
    ${SOURCES}
    ${HEADER}
    ${RESOURCES}
    )

target_link_libraries(scpimodule 
    PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Network
    Qt5::SerialPort
    VeinMeta::VfEvent
    VeinMeta::VfHelpers
    VeinMeta::VfComponent
    #MeasurementModuleInterface
    zerabasemodule
    zeraproxy
    zeramisc
    zeraxmlconfig
    zeravalidator 
    Zera::SCPI
    )

# announce headers - target perspective
target_include_directories(scpimodule
    PUBLIC
    $<INSTALL_INTERFACE:include/scpimodule>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

# install library
install(TARGETS scpimodule
    EXPORT scpimoduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/zera-modules
    )

# configure *ConfigVersion.cmake
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    scpimoduleConfigVersion.cmake
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
