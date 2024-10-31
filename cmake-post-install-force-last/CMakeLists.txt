if(NOT "${ZC_SERVICES_IP}" STREQUAL "")
    install(CODE
        "
        message(\"Replacing service IP addresses to ${ZC_SERVICES_IP}...\")
        execute_process (
            COMMAND sh -c \"sed -i 's:127.0.0.1:${ZC_SERVICES_IP}:g' `find ${SET_MODMAN_CONFIG_PATH} -name 'modulemanager_config*.json'`\"
            COMMAND_ERROR_IS_FATAL ANY
        )
        "
    )
endif()

