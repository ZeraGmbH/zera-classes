add_executable(zera-intel-hex-checker
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

target_include_directories(zera-intel-hex-checker
   PRIVATE
   $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
   ${CMAKE_SOURCE_DIR}/zera-math
   ${CMAKE_SOURCE_DIR}/zera-misc
)

target_link_libraries(zera-intel-hex-checker
    PRIVATE
    zera-intel-hex
    # ask linker to help us finding unresolved symbols
    "-Wl,--no-undefined"
)

install(TARGETS zera-intel-hex-checker
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
