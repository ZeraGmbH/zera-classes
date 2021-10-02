# set project name and language
project(zera-json-params LANGUAGES CXX
    VERSION ${REPO_VERSION}
)

# find required depemdencies
find_package(Qt5 COMPONENTS Core CONFIG REQUIRED)

file(GLOB HEADER
    ${PROJECT_SOURCE_DIR}/*.h
    )

# create library 
add_library(zera-json-params SHARED
    ${HEADER}
    ${CMAKE_CURRENT_SOURCE_DIR}/zera-json-params-structure.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/zera-json-params-state.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/zera-json-merge.cpp
)

# announce headers - target perspective
target_include_directories(zera-json-params
    PUBLIC
    $<INSTALL_INTERFACE:include/zera-classes/zera-json-params>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

# set target Version
set_target_properties(zera-json-params PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zera-json-params PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

target_link_libraries(zera-json-params
    PRIVATE
    Qt5::Core
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

# generate export header
include(GenerateExportHeader)
generate_export_header(zera-json-params)


# install export header
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zera-json-params_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/zera-json-params
    )

# install library
install(TARGETS zera-json-params
    EXPORT zera-json-paramsExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

# configure *ConfigVersion.cmake
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    zera-json-paramsConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMinorVersion
    )

# configure *Config.cmake
configure_file(zera-json-paramsConfig.cmake.in zera-json-paramsConfig.cmake @ONLY)


# install *Config(Version).cmake
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/zera-json-paramsConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/zera-json-paramsConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zera-json-params
    )

# install targets cmake-files
install(EXPORT zera-json-paramsExport
    FILE zera-json-paramsTargets.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zera-json-params
    )

# install public headers
install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/zera-json-params
    )




# create executable
add_executable(zera-json-params-cli
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

target_include_directories(zera-json-params-cli
   PRIVATE
   $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

# link dependencies
target_link_libraries(zera-json-params-cli
    PRIVATE
    Qt5::Core
    zera-json-params
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
)

# install application
install(TARGETS zera-json-params-cli
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
