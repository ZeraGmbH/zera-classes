get_directory_property( DirDefs DIRECTORY ${CMAKE_SOURCE_DIR} COMPILE_DEFINITIONS )
foreach( d ${DirDefs} )
    message( STATUS "Found Define: " ${d} )
endforeach()

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/modulemanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/moduleeventhandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/jsonsessionloader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/licensesystem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/customerdatasystem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/priorityarbitrationsystem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/zeradblogger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/jsonloggercontentloader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/modulemanagerconfig.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/diriteratorworker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/jsonloggercontentsessionloader.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    )

file(GLOB RESOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.qrc
    )

add_library(zera-modulemanager-lib STATIC
    ${SOURCES}
    ${HEADER}
    )

target_include_directories(zera-modulemanager-lib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    )

target_link_libraries(zera-modulemanager-lib
    PUBLIC
    Qt5::Core
    Qt5::Network
    Qt5::Concurrent
    Zera::VfEvent
    Zera::VfComponent
    Zera::VfStorageHash
    Zera::VfNet2
    Zera::VfTcp
    Zera::VfQml
    Zera::VfCpp
    Zera::ModmanBase
    Zera::VfCryptoBridge
    VeinMeta::VfScript
    VeinMeta::VfLogger
    VeinMeta::VfExport
    VeinMeta::VfFiles
    module-qt-plugin-interface
    zera-json-params
    )

add_executable(zera-modulemanager
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${RESOURCES}
    )

target_link_libraries(zera-modulemanager
    PRIVATE
    zera-modulemanager-lib
)

file(GLOB SESSION_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/sessions/*.json
)

set(CONFIG_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/modulemanager_config.json
    ${CMAKE_CURRENT_SOURCE_DIR}/modulemanager_config_dev.json
)

file(GLOB LOGGER_CONFIG_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/loggerconfigs/*session*.json
)

set(LOGGER_CONFIGS_SCHEMA ${CMAKE_CURRENT_SOURCE_DIR}/json-schemas/logger-configs-schema.json)
set(SESSION_SCHEMA ${CMAKE_CURRENT_SOURCE_DIR}/json-schemas/session-schema.json)

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
