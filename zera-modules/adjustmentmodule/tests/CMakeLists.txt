find_package(VfStorageHash REQUIRED)
set(SET_MODMAN_CONFIG_PATH  "${CMAKE_INSTALL_SYSCONFDIR}/zera/modules/")
add_definitions(-DMODMAN_CONFIG_PATH="${SET_MODMAN_CONFIG_PATH}")

SETUP_QTESTS(
    test_adjustmentmoduleactivator
)

target_link_libraries(test_adjustmentmoduleactivator
    PUBLIC
    adjustmentmodule-static
)
