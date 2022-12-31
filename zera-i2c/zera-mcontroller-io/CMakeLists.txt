add_executable(zera-mcontroller-io
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

target_link_libraries(zera-mcontroller-io
    Qt5::Core
    zera-i2c-devices
    zerai2c
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
)

install(TARGETS zera-mcontroller-io
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
