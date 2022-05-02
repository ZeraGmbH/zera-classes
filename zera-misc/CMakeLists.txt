if(ZC_BUILD_MODULES)
    find_package(Qt5 COMPONENTS Network CONFIG REQUIRED)

    # sourcefiles required to build modules
    set(SOURCES_FOR_MODULE
        secinterface_p.cpp
        secinterface.cpp
        rminterface_p.cpp
        dspinterface_p.cpp
        ethadress.cpp
        pcbinterface.cpp
        ethadress_p.cpp
        rminterface.cpp
        interface_p.cpp
        pcbinterface_p.cpp
        dspmeasdata.cpp
        dspinterface.cpp
    )
    # header required to build modules
    set(HEADER_FOR_MODULE
        dspvar.h
        secinterface.h
        rminterface.h
        interface_p.h
        dspmeasdata.h
        dspinterface.h
        dspinterface_p.h
        rminterface_p.h
        secinterface_p.h
        ethadress_p.h
        ethadress.h
        pcbinterface.h
        pcbinterface_p.h
    )
endif(ZC_BUILD_MODULES)

# common + optional sources
set(SOURCES
    intelhexfileio.cpp
    intelhexfileio_p.cpp
    ${SOURCES_FOR_MODULE}
)

# common + optional sources
set(HEADER
    zera_misc_global.h
    intelhexfileio.h
    intelhexfileio_p.h
    ${HEADER_FOR_MODULE}
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
    PRIVATE
    Qt5::Core
    ${DEPS_FOR_MODULE}
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
)

# announce headers - target perspective
target_include_directories(zeramisc
    PUBLIC
    $<INSTALL_INTERFACE:include/zera-classes/misc>
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

# configure *ConfigVersion.cmake
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    zeramiscConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMinorVersion
    )

# configure *Config.cmake
configure_file(zeramiscConfig.cmake.in zeramiscConfig.cmake @ONLY)

# install *Config(Version).cmake
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/zeramiscConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/zeramiscConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zeramisc
    )

# install targets cmake-files
install(EXPORT zeramiscExport
    FILE zeramiscTargets.cmake
    NAMESPACE ZeraClasses::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/zeramisc
    )

# install public headers
install(
    FILES ${HEADER}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/misc
    )
