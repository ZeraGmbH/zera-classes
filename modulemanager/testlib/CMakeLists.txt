file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(zera-modulemanager-testlib STATIC
    ${SOURCES}
    ${HEADER}
)

target_include_directories(zera-modulemanager-testlib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

add_definitions(-DOE_INSTALL_ROOT="${OE_INSTALL_ROOT}")

target_link_libraries(zera-modulemanager-testlib
    PUBLIC
    Qt5::Core
    Zera::zera-timers-testlib
    Zera::zenuxservicecommon-testlib
    zera-modulemanager-lib
    module-actual-values-testlib
)

set_target_properties(zera-modulemanager-testlib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zera-modulemanager-testlib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

