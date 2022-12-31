add_executable(zera-intel-hex-checker
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

target_link_libraries(zera-intel-hex-checker
    zera-intel-hex
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
)

install(TARGETS zera-intel-hex-checker
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
