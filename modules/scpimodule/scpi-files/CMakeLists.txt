file(GLOB SCPI_LOCAL_FILES_EXAMPLES
    *.txt
)

add_custom_target(scpi-file-examples ALL
    SOURCES ${SCPI_LOCAL_FILES_EXAMPLES}
    )

install(
    FILES ${SCPI_LOCAL_FILES_EXAMPLES}
    DESTINATION ${OPERATOR_HOME}/scpi-files/
    )
