file(GLOB HTML_PROSA
    ${CMAKE_CURRENT_SOURCE_DIR}/html-prosa/*.html
    ${CMAKE_CURRENT_SOURCE_DIR}/html-prosa/measurement-POW/*.html
)

file(GLOB XSL_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/xsl-files/templates/*.xsl
    ${CMAKE_CURRENT_SOURCE_DIR}/xsl-files/*.xsl
)

add_custom_target(html-prosa ALL
    SOURCES ${HTML_PROSA}
)

add_custom_target(xsl-files ALL
    SOURCES ${XSL_FILES}
)

add_custom_target(create-html ALL
    SOURCES create-html
)
