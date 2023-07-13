file(GLOB SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    )

file(GLOB RESOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.qrc
    )

file(GLOB CONFIG_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.xml
)

file(GLOB CONFIG_VALIDATOR
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.xsd
)

# check for valid config files / display in qt-creator
add_custom_target(spm1module-configs ALL
    DEPENDS ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    SOURCES ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    COMMAND xmllint --noout --schema ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    )

add_library(spm1module MODULE
    ${SOURCES}
    ${HEADER}
    ${RESOURCES}
    )

target_link_libraries(spm1module 
    PRIVATE
    zera-basemodule
    sec-modules-common
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(spm1module
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )

install(TARGETS spm1module
    EXPORT spm1moduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/zera-modules
    )

install(
    FILES ${CONFIG_FILES} ${CONFIG_VALIDATOR}
    DESTINATION ${SET_ZC_CONFIG_PATH}
    )
