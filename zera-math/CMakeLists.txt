file(GLOB SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADER 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

add_library(zeramath SHARED
    ${SOURCES}
    ${HEADER}
    )

target_link_libraries(zeramath 
    PRIVATE
    Qt5::Core
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

target_include_directories(zeramath
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/math>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

set_target_properties(zeramath PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zeramath PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(zeramath)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zeramath_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/math
    )

install(TARGETS zeramath
    EXPORT zeramathExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

write_basic_package_version_file(
    zeramathConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMajorVersion
    )

configure_file(zeramathConfig.cmake.in zeramathConfig.cmake @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/zeramathConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/zeramathConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zeramath
    )

install(EXPORT zeramathExport
    FILE zeramathTargets.cmake
    NAMESPACE ZeraClasses::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zeramath
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/math
    )
