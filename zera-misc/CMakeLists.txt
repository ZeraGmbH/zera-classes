if(ZC_BUILD_MODULES)
    set(SOURCES_FOR_MODULE
        ethadress.cpp
        ethadress_p.cpp
    )
    set(HEADER_FOR_MODULE
        ethadress_p.h
        ethadress.h
        singleonoffreferencemanager.h
)
endif(ZC_BUILD_MODULES)

set(SOURCES
    ${SOURCES_FOR_MODULE}
    intelhexfileio.cpp
    intelhexfileio_p.cpp
)

set(HEADER
    ${HEADER_FOR_MODULE}
    intelhexfileio.h
    intelhexfileio_p.h
)

add_library(zeramisc SHARED
    ${SOURCES}
    ${HEADER}
)

target_link_libraries(zeramisc 
    PUBLIC
    Qt5::Core
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
)

target_include_directories(zeramisc
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/misc>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )

set_target_properties(zeramisc PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(zeramisc PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

generate_export_header(zeramisc)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/zeramisc_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/misc
)

install(TARGETS zeramisc
    EXPORT zeramiscExport
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

write_basic_package_version_file(
    zeramiscConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMajorVersion
    )

configure_file(zeramiscConfig.cmake.in zeramiscConfig.cmake @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/zeramiscConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/zeramiscConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zeramisc
    )

install(EXPORT zeramiscExport
    FILE zeramiscTargets.cmake
    NAMESPACE ZeraClasses::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zeramisc
    )

install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/misc
    )

if(ZC_BUILD_MODULES)
    add_subdirectory(tests)
endif()
