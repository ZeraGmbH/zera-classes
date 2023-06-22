cmake_minimum_required(VERSION 3.1)

project(zera-modulemanager LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(zeramodulemanager_VERSION_MAJOR "1")
set(zeramodulemanager_VERSION_MINOR "2")
set(zeramodulemanager_VERSION_PATCH "0")
set(PROJECT_VERSION "${zeramodulemanager_VERSION_MAJOR}.${zeramodulemanager_VERSION_MINOR}.${zeramodulemanager_VERSION_PATCH}")

include(FeatureSummary)
include(GNUInstallDirs)

find_package(Qt5 COMPONENTS Core Network Qml Concurrent Test CONFIG REQUIRED)
find_package(VfEvent REQUIRED)
find_package(VfComponent REQUIRED)
find_package(VfStorageHash REQUIRED)
find_package(VfTcp REQUIRED)
find_package(VfNet2 REQUIRED)
find_package(VfHelpers REQUIRED)
find_package(VfScript REQUIRED)
find_package(VfLogger REQUIRED)
find_package(VfCryptoBridge REQUIRED)
find_package(VfExport REQUIRED)
find_package(VfFiles REQUIRED)
find_Package(ZeraClasses REQUIRED)
find_Package(VfCpp REQUIRED)
find_package(zera-json-params REQUIRED)

set(SET_MODMAN_SESSION_PATH "${CMAKE_INSTALL_SYSCONFDIR}/zera/modules/sessions/")
set(SET_MODMAN_CONFIG_PATH  "${CMAKE_INSTALL_SYSCONFDIR}/zera/modules/")
set(SET_MODMAN_MODULE_PATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/zera-modules/")
set(SET_MODMAN_CONTENTSET_PATH "${CMAKE_INSTALL_SYSCONFDIR}/zera/logger-contentsets/")
set(SET_MODMAN_CUSTOMERDATA_PATH "/home/operator/customerdata/" CACHE STRING "Customer data path")
set(SET_MODMAN_AUTOMOUNT_PATH "/media/" CACHE STRING "Automount path")
set(SET_MODMAN_LOGGER_LOCAL_PATH "/home/operator/logger" CACHE STRING "Logger local storage path")
set(SET_MODMAN_DEFAULT_SESSION "modulemanager_config.json" CACHE STRING "Default session")

add_definitions(-DMODMAN_SESSION_PATH="${SET_MODMAN_SESSION_PATH}")
add_definitions(-DMODMAN_CONFIG_PATH="${SET_MODMAN_CONFIG_PATH}")
add_definitions(-DMODMAN_CONFIG_FILE="${SET_MODMAN_CONFIG_PATH}/${SET_MODMAN_DEFAULT_SESSION}")
add_definitions(-DMODMAN_MODULE_PATH="${SET_MODMAN_MODULE_PATH}")
add_definitions(-DMODMAN_CUSTOMERDATA_PATH="${SET_MODMAN_CUSTOMERDATA_PATH}")
add_definitions(-DMODMAN_CONTENTSET_PATH="${SET_MODMAN_CONTENTSET_PATH}")
add_definitions(-DMODMAN_AUTOMOUNT_PATH="${SET_MODMAN_AUTOMOUNT_PATH}")
add_definitions(-DMODMAN_LOGGER_LOCAL_PATH="${SET_MODMAN_LOGGER_LOCAL_PATH}")

get_directory_property( DirDefs DIRECTORY ${CMAKE_SOURCE_DIR} COMPILE_DEFINITIONS )
foreach( d ${DirDefs} )
    message( STATUS "Found Define: " ${d} )
endforeach()

set(SOURCES
    ${PROJECT_SOURCE_DIR}/src/modulemanager.cpp
    ${PROJECT_SOURCE_DIR}/src/moduleeventhandler.cpp
    ${PROJECT_SOURCE_DIR}/src/jsonsessionloader.cpp
    ${PROJECT_SOURCE_DIR}/src/licensesystem.cpp
    ${PROJECT_SOURCE_DIR}/src/customerdatasystem.cpp
    ${PROJECT_SOURCE_DIR}/src/priorityarbitrationsystem.cpp
    ${PROJECT_SOURCE_DIR}/src/zeradblogger.cpp
    ${PROJECT_SOURCE_DIR}/src/modulemanagercontroller.cpp
    ${PROJECT_SOURCE_DIR}/src/jsonloggercontentloader.cpp
    ${PROJECT_SOURCE_DIR}/src/modulemanagerconfig.cpp
    ${PROJECT_SOURCE_DIR}/src/diriteratorworker.cpp
    ${PROJECT_SOURCE_DIR}/src/jsonloggercontentsessionloader.cpp
    )

file(GLOB HEADER
    ${PROJECT_SOURCE_DIR}/src/*.h
    )

file(GLOB RESOURCES 
    ${PROJECT_SOURCE_DIR}/*.qrc
    )

add_library(zera-modulemanager-lib STATIC
    ${SOURCES}
    ${HEADER}
    )

target_link_libraries(zera-modulemanager-lib
    PUBLIC
    Qt5::Core
    Qt5::Network
    Qt5::Concurrent
    VeinMeta::VfEvent
    VeinMeta::VfComponent
    VeinMeta::VfStorageHash
    VeinMeta::VfTcp
    VeinMeta::VfNet2
    VeinMeta::VfHelpers
    VeinMeta::VfQml
    VeinMeta::VfScript
    VeinMeta::VfLogger
    VeinMeta::VfCpp
    VeinMeta::VfExport
    VeinMeta::VfFiles
    VeinMeta::VfCryptoBridge
    Zera::module-qt-plugin-interface
    zera-json-params
    )

add_executable(zera-modulemanager
    ${PROJECT_SOURCE_DIR}/src/main.cpp
    ${RESOURCES}
    )

target_link_libraries(zera-modulemanager
    PRIVATE
    zera-modulemanager-lib
)

file(GLOB SESSION_FILES
    ${PROJECT_SOURCE_DIR}/sessions/*.json
)

set(CONFIG_FILES
    ${PROJECT_SOURCE_DIR}/modulemanager_config.json
    ${PROJECT_SOURCE_DIR}/modulemanager_config_dev.json
)

file(GLOB LOGGER_CONFIG_FILES
    ${PROJECT_SOURCE_DIR}/loggerconfigs/*session*.json
)
# Note 1: In OE we do not (yet) have jsonlint-php so it must be optional
# Note 2: In case we make COMMAND in target below conditional on JSONLINT,
#         qt-creator does not display source-capabilities. That's what the
#         'echo' dance is for
find_program (JSONLINT NAMES jsonlint-php echo)
if(${JSONLINT} MATCHES ".*echo.*")
    message(WARNING "Program jsonlint-php not found. On Fedora install php-jsonlint")
endif()

find_program (JSONSCHEMA NAMES jsonschema REQUIRED)

set(LOGGER_CONFIGS_SCHEMA ${PROJECT_SOURCE_DIR}/json-schemas/logger-configs-schema.json)
set(SESSION_SCHEMA ${PROJECT_SOURCE_DIR}/json-schemas/session-schema.json)

# targets to see files in qt-creator and check for validity
add_custom_target(json-schemas ALL
    SOURCES ${LOGGER_CONFIGS_SCHEMA} ${SESSION_SCHEMA}
    COMMAND ${JSONLINT} ${LOGGER_CONFIGS_SCHEMA} ${SESSION_SCHEMA}
    )

set(LOGGER_CONFIG_SCHEMA_CMDS)
foreach(config ${LOGGER_CONFIG_FILES})
  list(APPEND LOGGER_CONFIG_SCHEMA_CMDS
      COMMAND ${JSONSCHEMA} -i ${config} ${LOGGER_CONFIGS_SCHEMA} || (echo \"Error: Schema validation failed in '${config}'\" 1>&2$<SEMICOLON> exit 1)
    )
endforeach()

add_custom_target(logger-config-files ALL
    SOURCES ${LOGGER_CONFIG_FILES}
    COMMAND ${JSONLINT} ${LOGGER_CONFIG_FILES}
    ${LOGGER_CONFIG_SCHEMA_CMDS}
    )

set(SESSION_SCHEMA_CMDS)
foreach(config ${SESSION_FILES})
  list(APPEND SESSION_SCHEMA_CMDS
      COMMAND ${JSONSCHEMA} -i ${config} ${SESSION_SCHEMA} || (echo \"Error: Schema validation failed in '${config}'\" 1>&2$<SEMICOLON> exit 1)
    )
endforeach()

add_custom_target(session-files ALL
    SOURCES ${SESSION_FILES}
    COMMAND ${JSONLINT} ${SESSION_FILES}
    #${SESSION_SCHEMA_CMDS}
    )

add_custom_target(zera-modulemanager-config ALL
    SOURCES ${CONFIG_FILES}
    COMMAND ${JSONLINT} ${CONFIG_FILES}
    )

target_include_directories(zera-modulemanager
    PUBLIC
    $<BUILD_INTERFACE:${MY_LIBRARY_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-modulemanager>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

install(TARGETS zera-modulemanager
    EXPORT zera-modulemanagerExport
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

install(
    FILES ${SESSION_FILES}
    DESTINATION ${SET_MODMAN_SESSION_PATH}
    )

install(
    FILES ${CONFIG_FILES}
    DESTINATION ${SET_MODMAN_CONFIG_PATH}
    )

install(
    FILES ${LOGGER_CONFIG_FILES}
    DESTINATION ${SET_MODMAN_CONTENTSET_PATH}
    )

enable_testing()
add_subdirectory(tests)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
