file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tasks/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tasks/*.h
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
add_custom_target(adjustmentmodule-configs ALL
    DEPENDS ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    SOURCES ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    COMMAND xmllint --noout --schema ${CONFIG_VALIDATOR} ${CONFIG_FILES}
    )

add_library(adjustmentmodule-static STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(adjustmentmodule-static
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/tasks>
    )

target_link_libraries(adjustmentmodule-static
    zerabasemodule
    zeramath
    module-tasks
)

add_library(adjustmentmodule MODULE
    ${RESOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/module-gluelogic/adjustmentmodulefactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/module-gluelogic/adjustmentmodulefactory.h
    )

target_link_libraries(adjustmentmodule
    PRIVATE
    adjustmentmodule-static
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(adjustmentmodule
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

install(TARGETS adjustmentmodule
    EXPORT adjustmentmoduleExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/zera-modules
    )

install(
    FILES ${CONFIG_FILES} ${CONFIG_VALIDATOR}
    DESTINATION ${SET_ZC_CONFIG_PATH}
    )

add_subdirectory(tests)

