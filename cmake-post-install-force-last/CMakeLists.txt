if(NOT "${ZC_SERVICES_IP}" STREQUAL "")
    install(CODE
        "
        message(\"Replacing service IP addresses to ${ZC_SERVICES_IP}...\")
        execute_process (
            COMMAND sh -c \"sed -i 's:<ip>.*</ip>:<ip>${ZC_SERVICES_IP}</ip>:g' `find ${SET_MODMAN_CONFIG_PATH} -name '*.xml'`\"
            COMMAND_ERROR_IS_FATAL ANY
        )
        "
    )
endif()

