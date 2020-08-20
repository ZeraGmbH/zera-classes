# find required depemdencies
find_package(Qt5 COMPONENTS Core CONFIG REQUIRED)

# create executable
add_executable(zera-mcontroller-io
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

target_include_directories(zera-mcontroller-io
   PRIVATE
   $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
   ${CMAKE_SOURCE_DIR}/zera-dev
   ${CMAKE_SOURCE_DIR}/zera-math
   ${CMAKE_SOURCE_DIR}/zera-misc
)

# link dependencies
target_link_libraries(zera-mcontroller-io
    PRIVATE
    Qt5::Core
    zeradev
    zeramath
    zeramisc
    zerai2c
)

# install application
install(TARGETS zera-mcontroller-io
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
