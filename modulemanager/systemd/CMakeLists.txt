set(SYSTEMD_UNITS
    modulemanager.service
    modulemanager.socket
    )
add_custom_target(modulemanager-systemd-units ALL
    DEPENDS ${SYSTEMD_UNITS}
    SOURCES ${SYSTEMD_UNITS}
    COMMAND echo "Install systemd units..."
    )
install(
    FILES ${SYSTEMD_UNITS}
    DESTINATION ${SYSTEMD_UNIT_INSTALL_PATH}
    )
