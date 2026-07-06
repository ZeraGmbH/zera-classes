file(GLOB SOURCES
    *.cpp
    tasks/*.cpp
    )

file(GLOB HEADER
    *.h
    tasks/*.h
    )

add_library(rangemodule-lib SHARED
    ${SOURCES}
    ${HEADER}
)

target_include_directories(rangemodule-lib
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tasks>
    )

target_link_libraries(rangemodule-lib
    PUBLIC
    zera-basemodule
    Zera::zera-timers
    Zera::zera-log-statistics
)
if("${QT_MAJOR_VER}" STREQUAL "6")
    target_link_libraries(rangemodule-lib
        PUBLIC
            Qt6::Core5Compat
    )
endif()

set_target_properties(rangemodule-lib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(rangemodule-lib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

export(TARGETS rangemodule-lib NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS rangemodule-lib
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
