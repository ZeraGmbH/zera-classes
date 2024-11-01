file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(vf-storage SHARED
    ${SOURCES}
    ${HEADERS}
)

target_link_libraries(vf-storage
    PUBLIC
    Zera::VfCpp
    Zera::VfStorageHash
)

target_include_directories(vf-storage
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/vf-storage>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

set_target_properties(vf-storage PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(vf-storage PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(vf-storage)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/vf-storage_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/vf-storage
    )

export(TARGETS vf-storage NAMESPACE Zera:: FILE ZeraClassesTargets.cmake)
install(TARGETS vf-storage
    EXPORT ZeraClassesExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/vf-storage
    )

add_subdirectory(tests)
