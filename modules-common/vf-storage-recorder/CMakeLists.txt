file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(vf-storage-recorder SHARED
    ${SOURCES}
    ${HEADERS}
)

target_link_libraries(vf-storage-recorder
    PUBLIC
    Zera::VfCpp
    Zera::VfStorage
)

target_include_directories(vf-storage-recorder
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/vf-storage-recorder>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

set_target_properties(vf-storage-recorder PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(vf-storage-recorder PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(vf-storage-recorder)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/vf-storage-recorder_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/vf-storage-recorder
    )

export(TARGETS vf-storage-recorder NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS vf-storage-recorder
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/vf-storage-recorder
    )

add_subdirectory(tests)
