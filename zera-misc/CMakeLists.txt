if(ZC_BUILD_MODULES)
    file(GLOB SOURCES_FOR_MODULE
        channels/*.cpp
        service-interfaces/*.cpp
        )
    set(SOURCES_FOR_MODULE
        ${SOURCES_FOR_MODULE}
        ethadress.cpp
        ethadress_p.cpp
    )
    file(GLOB HEADER_FOR_MODULE
        channels/*.h
        service-interfaces/*.h
        )
    set(HEADER_FOR_MODULE
        ${HEADER_FOR_MODULE}
        ethadress_p.h
        ethadress.h
        errormessages.h
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

if(ZC_BUILD_MODULES)
    set(DEPS_FOR_MODULE
        Qt5::Network
        zeraproxy
    )
endif(ZC_BUILD_MODULES)

target_link_libraries(zeramisc 
    PUBLIC
    Qt5::Core
    ${DEPS_FOR_MODULE}
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
)

target_include_directories(zeramisc
    PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/misc>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/channels>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/service-interfaces>
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

include(CMakePackageConfigHelpers)
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
