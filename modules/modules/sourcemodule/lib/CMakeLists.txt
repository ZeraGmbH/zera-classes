file(GLOB SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/io-device/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/io-queue/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/json/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/message-texts/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/module-gluelogic/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source-device/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source-protocols/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source-scanner/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/transaction-ids/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vein/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/io-device/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/io-queue/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/json/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/message-texts/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/module-gluelogic/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/source-device/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/source-protocols/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/source-scanner/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/transaction-ids/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/vein/*.h
    )

file(GLOB RESOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.qrc
    )

file(GLOB SOURCE_DEVINFO
    ${CMAKE_CURRENT_SOURCE_DIR}/deviceinfo/*.json
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

add_library(sourcemodule-static STATIC
    ${SOURCES}
    ${HEADER}
    ${RESOURCES}
    )

target_link_libraries(sourcemodule-static
    PUBLIC
    vf-cpp-module-wrappers
    zera-basemodule
    serialportasyncblock
    Zera::zera-timers
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(sourcemodule-static
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/io-device>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/io-queue>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/json>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/message-texts>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/module-gluelogic>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/source-device>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/source-protocols>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/source-scanner>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/transaction-ids>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/vein>
    )
