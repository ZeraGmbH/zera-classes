add_test(NAME    sourcemodule-unittest
         COMMAND sourcemodule-unittest)

file(GLOB UNITTESTS
    *.cpp
)

add_executable(sourcemodule-unittest
    ${UNITTESTS}
    ${CMAKE_CURRENT_BINARY_DIR}/../../sourcemodule-config.cxx
)
# constexpr / EXPECT_EQ -> linker problems for c++ < 17
set_property(TARGET sourcemodule-unittest PROPERTY CXX_STANDARD 17)

target_include_directories(sourcemodule-unittest
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

target_link_libraries(sourcemodule-unittest
    PUBLIC
    gtest
    gtest_main
    sourcemodule-static
)
