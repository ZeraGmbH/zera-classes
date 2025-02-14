file(GLOB SOURCES
    *.cpp
    tasks/*.cpp
    )

file(GLOB HEADER
    *.h
    tasks/*.h
    )

add_library(adjustmentmodule-static STATIC
    adjustmentmodulexmlschema.qrc
    ${SOURCES}
    ${HEADER}
)

target_include_directories(adjustmentmodule-static
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tasks>
    )

target_link_libraries(adjustmentmodule-static
    zera-basemodule
    Zera::zera-math
    module-tasks
)

