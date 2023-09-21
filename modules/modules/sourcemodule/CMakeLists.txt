file(GLOB SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/io-device/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/io-queue/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/json/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/message-texts/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/module-gluelogic/*.cpp
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
    ${CMAKE_CURRENT_SOURCE_DIR}/src/module-gluelogic/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/source-device/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/source-protocols/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/source-scanner/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/transaction-ids/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vein/*.h
    )

file(GLOB RESOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.qrc
    )

file(GLOB SOURCE_DEVINFO
    ${CMAKE_CURRENT_SOURCE_DIR}/deviceinfo/*.json
)

# auto-generate c++ xml access from xsd
file(GLOB XSL
    ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.xsd
)

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/sourcemodule-config.hxx
        ${CMAKE_CURRENT_BINARY_DIR}/sourcemodule-config.cxx
    COMMAND
        xsdcxx cxx-tree --std c++11 --generate-serialization --output-dir "${CMAKE_CURRENT_BINARY_DIR}" ${XSL}
    DEPENDS
        ${XSL}
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
    ${CMAKE_CURRENT_BINARY_DIR}/sourcemodule-config.cxx
    ${CMAKE_CURRENT_BINARY_DIR}/sourcemodule-config.hxx
    ${SOURCES}
    ${HEADER}
    ${RESOURCES}
    )

target_link_libraries(sourcemodule-static
    PUBLIC
    vf-cpp-module-wrappers
    ${COMMON_LIBS}
    )

target_include_directories(sourcemodule-static
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/io-device>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/io-queue>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/json>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/message-texts>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/module-gluelogic>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/source-device>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/source-protocols>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/source-scanner>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/transaction-ids>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/vein>
    )

add_subdirectory(configs)
add_subdirectory(module)
add_subdirectory(testlib)
add_subdirectory(tests)
