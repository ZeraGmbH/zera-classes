file(GLOB SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
     ${CMAKE_CURRENT_SOURCE_DIR}/*.h
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
        ${CMAKE_CURRENT_BINARY_DIR}/electronicburdenmodule-config.hxx
        ${CMAKE_CURRENT_BINARY_DIR}/electronicburdenmodule-config.cxx
    COMMAND
        xsdcxx cxx-tree --std c++11 --generate-serialization --output-dir "${CMAKE_CURRENT_BINARY_DIR}" ${CONFIG_VALIDATOR}
    DEPENDS
        ${CONFIG_VALIDATOR}
)

add_custom_target(electronicburdenmodule-config-wrappers ALL
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/electronicburdenmodule-config.hxx
        ${CMAKE_CURRENT_BINARY_DIR}/electronicburdenmodule-config.cxx
)

set_property(
    SOURCE
        ${CMAKE_CURRENT_BINARY_DIR}/electronicburdenmodule-config.hxx
        ${CMAKE_CURRENT_BINARY_DIR}/electronicburdenmodule-config.cxx
    PROPERTY SKIP_AUTOMOC ON
)


# check for valid config files / display in qt-creator
add_custom_target(electronicburdenmodule-configs ALL
    DEPENDS ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    SOURCES ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    COMMAND xmllint --noout --schema ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    )

# Note 1: In OE we do not (yet) have jsonlint-php so it must be optional
# Note 2: In case we make COMMAND in target below conditional on JSONLINT,
#         qt-creator does not display electronicburden-deviceinfo. That's what the
#         'echo' dance is for
find_program (JSONLINT NAMES jsonlint-php echo)
if(${JSONLINT} MATCHES ".*echo.*")
    message(WARNING "Program jsonlint-php not found. On Fedora install php-jsonlint")
endif()

# check for valid devinfo / display in qt-creator
add_custom_target(electronicburden-deviceinfo-dont-forget-qrc ALL
    SOURCES ${SOURCE_DEVINFO}
    COMMAND ${JSONLINT} ${SOURCE_DEVINFO}
    )

# check if all configs made it into resource.qrc
foreach(electronicburdenconfigfile ${SOURCE_DEVINFO})
    get_filename_component(filename ${electronicburdenconfigfile} NAME)
    file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/resource.qrc lines REGEX "${filename}")
    if(NOT lines)
        message(FATAL_ERROR "${filename} was not added to resource.qrc - electronic burden will not work!!")
    endif()
endforeach()

set(COMMON_LIBS
    ${XERCES_LIBRARIES}
    zera-basemodule
    serialportasyncblock
    )

# create library
add_library(electronicburdenmodule MODULE
    ${SOURCES}
    ${HEADER}
    ${RESOURCES}
    ${CMAKE_CURRENT_BINARY_DIR}/electronicburdenmodule-config.cxx
    )

target_link_libraries(electronicburdenmodule
    PUBLIC
    ${COMMON_LIBS}
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(electronicburdenmodule
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/electronicburdenmodule>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

target_include_directories(electronicburdenmodule
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

install(TARGETS electronicburdenmodule
    EXPORT electronicburdenmoduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/zera-modules
    )

install(
    FILES
        ${CONFIG_FILES}
        ${CONFIG_VALIDATOR}
    DESTINATION
        ${SET_ZC_CONFIG_PATH}
    )
