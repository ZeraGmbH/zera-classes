add_executable(zera-i2c-ping
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

target_link_libraries(zera-i2c-ping
    Qt5::Core
    zerai2c
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
    )

install(TARGETS zera-i2c-ping
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
