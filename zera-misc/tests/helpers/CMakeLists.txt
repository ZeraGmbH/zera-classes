file(GLOB HELPER_SOURCES
    *.h
    *.cpp
)

add_library(helpers STATIC
    ${HELPER_SOURCES}
    )

target_include_directories(helpers
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/helpers>
    )

target_link_libraries(helpers
    PUBLIC
    Qt5::Core
    Qt5::Network
    Qt5::Test
    zeramisc
    zeraproxy
    )

