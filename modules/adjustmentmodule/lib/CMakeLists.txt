file(GLOB SOURCES
    *.cpp
    tasks/*.cpp
    )

file(GLOB HEADER
    *.h
    tasks/*.h
    )

add_library(adjustmentmodule-lib SHARED
    ${SOURCES}
    ${HEADER}
)

target_include_directories(adjustmentmodule-lib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tasks>
    )

target_link_libraries(adjustmentmodule-lib
    PUBLIC
    zera-basemodule
    service-common
    Zera::zera-math
    module-tasks
)

set_target_properties(adjustmentmodule-lib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(adjustmentmodule-lib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS adjustmentmodule-lib NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS adjustmentmodule-lib
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
