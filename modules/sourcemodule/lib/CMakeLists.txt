file(GLOB SOURCES 
    io-device/*.cpp
    io-queue/*.cpp
    message-texts/*.cpp
    module-gluelogic/*.cpp
    source-device-ext-serial/*.cpp
    source-device-internal/*.cpp
    source-device-common/*.cpp
    source-protocols/*.cpp
    source-scanner/*.cpp
    transaction-ids/*.cpp
    )

file(GLOB HEADER
    io-device/*.h
    io-queue/*.h
    message-texts/*.h
    module-gluelogic/*.h
    source-device-ext-serial/*.h
    source-device-internal/*.h
    source-device-common/*.h
    source-protocols/*.h
    source-scanner/*.h
    transaction-ids/*.h
    )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC" )

add_library(sourcemodule-static STATIC
    ${SOURCES}
    ${HEADER}
    source-device-ext-serial/source_device_info.qrc
    )

target_link_libraries(sourcemodule-static
    PUBLIC
    vf-cpp-module-wrappers
    zera-basemodule
    serialportasyncblock
    Zera::source-json-io-lib
    Zera::zera-timers
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(sourcemodule-static
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/io-device>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/io-queue>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/message-texts>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/module-gluelogic>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/source-device-ext-serial>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/source-device-internal>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/source-device-common>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/source-protocols>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/source-scanner>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/transaction-ids>
    )

add_definitions(-DZC_DEV_STATE_PATH="${CMAKE_INSTALL_SYSCONFDIR}/zera/states")

file(GLOB SOURCE_DEVINFO
    source-device-ext-serial/deviceinfo/*.json
)

# Note 1: In OE we do not (yet) have jsonlint-php so it must be optional
# Note 2: In case we make COMMAND in target below conditional on JSONLINT,
#         qt-creator does not display source-capabilities. That's what the
#         'echo' dance is for
find_program(JSONLINT NAMES jsonlint-php echo)
if(${JSONLINT} MATCHES ".*echo.*")
    message(WARNING "Program jsonlint-php not found. On Fedora install php-jsonlint")
endif()

# check for valid devinfo / display in qt-creator
add_custom_target(source-deviceinfo-dont-forget-qrc ALL
    SOURCES ${SOURCE_DEVINFO}
    COMMAND ${JSONLINT} ${SOURCE_DEVINFO}
    )

# check if all configs made it into source_device_info.qrc
foreach(sourceconfigfile ${SOURCE_DEVINFO})
    get_filename_component(filename ${sourceconfigfile} NAME)
    file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/source-device-ext-serial/source_device_info.qrc lines REGEX "${filename}")
    if(NOT lines)
        message(FATAL_ERROR "${filename} was not added to source_device_info.qrc - source will not work!!")
    endif()
endforeach()
