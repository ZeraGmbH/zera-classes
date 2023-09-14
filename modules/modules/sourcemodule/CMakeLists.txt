file(GLOB SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/io-device/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/io-queue/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/json/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/message-texts/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/source-device/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/source-protocols/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/source-scanner/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/transaction-ids/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vein/*.cpp
    )


file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/io-device/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/io-queue/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/json/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/message-texts/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/source-device/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/source-protocols/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/source-scanner/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/transaction-ids/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vein/*.h
    )

file(GLOB RESOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.qrc
    )

file(GLOB CONFIG_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.xml
)

file(GLOB CONFIG_VALIDATOR
    ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.xsd
)

file(GLOB SOURCE_DEVINFO
    ${CMAKE_CURRENT_SOURCE_DIR}/deviceinfo/*.json
)

# auto-generate c++ xml access from xsd
add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/sourcemodule-config.hxx
        ${CMAKE_CURRENT_BINARY_DIR}/sourcemodule-config.cxx
    COMMAND
        xsdcxx cxx-tree --std c++11 --generate-serialization --output-dir "${CMAKE_CURRENT_BINARY_DIR}" ${CONFIG_VALIDATOR}
    DEPENDS
        ${CONFIG_VALIDATOR}
)

add_custom_target(sourcemodule-config-wrappers ALL
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/sourcemodule-config.hxx
        ${CMAKE_CURRENT_BINARY_DIR}/sourcemodule-config.cxx
)

set_property(
    SOURCE
        ${CMAKE_CURRENT_BINARY_DIR}/sourcemodule-config.hxx
        ${CMAKE_CURRENT_BINARY_DIR}/sourcemodule-config.cxx
    PROPERTY SKIP_AUTOMOC ON
)


# check for valid config files / display in qt-creator
add_custom_target(sourcemodule-configs ALL
    DEPENDS ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    SOURCES ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    COMMAND xmllint --noout --schema ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    )

# check for valid devinfo / display in qt-creator
add_custom_target(source-deviceinfo-dont-forget-qrc ALL
    SOURCES ${SOURCE_DEVINFO}
    COMMAND ${JSONLINT} ${SOURCE_DEVINFO}
    )

# check if all configs made it into resource.qrc
foreach(sourceconfigfile ${SOURCE_DEVINFO})
    get_filename_component(filename ${sourceconfigfile} NAME)
    file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/resource.qrc lines REGEX "${filename}")
    if(NOT lines)
        message(FATAL_ERROR "${filename} was not added to resource.qrc - source will not work!!")
    endif()
endforeach()

set(COMMON_LIBS
    ${XERCES_LIBRARIES}
    zera-basemodule
    serialportasyncblock
    Zera::zera-timers
    )

add_library(sourcemodule-static STATIC
    ${SOURCES}
    ${HEADER}
    ${RESOURCES}
    )

target_link_libraries(sourcemodule-static
    PUBLIC
    ${COMMON_LIBS}
    )

target_include_directories(sourcemodule-static
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/io-device>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/io-queue>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/json>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/message-texts>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/source-device>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/source-protocols>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/source-scanner>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/transaction-ids>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/vein>
    )

file(GLOB MODULE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/module-gluelogic/*.cpp
    )

file(GLOB MODULE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/module-gluelogic/*.h
    )

add_library(sourcemodule MODULE
    ${MODULE_SOURCES}
    ${MODULE_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/sourcemodule-config.cxx
    )

target_link_libraries(sourcemodule
    PUBLIC
    sourcemodule-static
    vf-cpp-module-wrappers
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

install(TARGETS sourcemodule
    EXPORT sourcemoduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/zera-modules
    )

install(
    FILES
        ${CONFIG_FILES}
        ${CONFIG_VALIDATOR}
    DESTINATION
        ${SET_ZC_CONFIG_PATH}
    )

if(NOT SKIP_UNITTESTS)
    add_subdirectory(tests)
endif()

if(NOT SKIP_UNITTESTS)
    add_test(NAME    sourcemodule-unittest
             COMMAND sourcemodule-unittest)

    file(GLOB UNITTESTS
        tests/gtest/unittest-*.cpp
    )

    add_executable(sourcemodule-unittest
        ${UNITTESTS}
        ${CMAKE_CURRENT_BINARY_DIR}/sourcemodule-config.cxx
    )
    # constexpr / EXPECT_EQ -> linker problems for c++ < 17
    set_property(TARGET sourcemodule-unittest PROPERTY CXX_STANDARD 17)

    target_include_directories(sourcemodule-unittest
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        )

    target_link_libraries(sourcemodule-unittest
        PUBLIC
        gtest
        gtest_main
        sourcemodule-static
    )
endif()
