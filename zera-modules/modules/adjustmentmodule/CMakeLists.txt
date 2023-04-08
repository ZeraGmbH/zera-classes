# xml-schema resource is required by module and tests
# If we put resource into our library both would need some call of
# Q_INIT_RESOURCE. So do publish resource and compile in where necesary
set(XML_SCHEMA_RESOURCE ${CMAKE_CURRENT_SOURCE_DIR}/configs/xml-schema.qrc)

add_subdirectory(configs)
add_subdirectory(lib)
add_subdirectory(module)
add_subdirectory(tests)
