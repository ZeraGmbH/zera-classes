if(ZC_BUILD_MODULES)
    set(SOURCES_FOR_MODULE
        ethadress.cpp
        ethadress_p.cpp
        service-interfaces/dspinterface_p.cpp
        service-interfaces/dspinterface.cpp
        service-interfaces/dspmeasdata.cpp
        service-interfaces/interface_p.cpp
        service-interfaces/pcbinterface.cpp
        service-interfaces/pcbinterface_p.cpp
        service-interfaces/rminterface.cpp
        service-interfaces/rminterface_p.cpp
        service-interfaces/secinterface.cpp
        service-interfaces/secinterface_p.cpp
    )
    set(HEADER_FOR_MODULE
        service-interfaces/dspvar.h
        service-interfaces/secinterface.h
        service-interfaces/rminterface.h
        service-interfaces/interface_p.h
        service-interfaces/dspmeasdata.h
        service-interfaces/dspinterface.h
        service-interfaces/dspinterface_p.h
        service-interfaces/rminterface_p.h
        service-interfaces/secinterface_p.h
        ethadress_p.h
        ethadress.h
        singleonoffreferencemanager.h
        service-interfaces/pcbinterface.h
        service-interfaces/pcbinterface_p.h
)
endif(ZC_BUILD_MODULES)

# common
set(SOURCES
    intelhexfileio.cpp
    intelhexfileio_p.cpp
    task-runner/taskcomposite.cpp
    task-runner/taskextraerrorhandler.cpp
    task-runner/taskparallel.cpp
    task-runner/tasksequence.cpp
    task-runner/tasktimeoutdecorator.cpp
    ${SOURCES_FOR_MODULE}
)

set(HEADER
    intelhexfileio.h
    intelhexfileio_p.h
    task-runner/taskcomposit.h
    task-runner/taskcontainer.h
    task-runner/taskextraerrorhandler.h
    task-runner/taskparallel.h
    task-runner/tasksequence.h
    task-runner/tasktimeoutdecorator.h
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
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/zera-classes/misc>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/service-interfaces>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/task-runner>
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

add_subdirectory(tests)
