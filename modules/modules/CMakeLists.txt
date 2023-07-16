macro(SET_MODULE_ENV MODULE_NAME)
    string(TOUPPER ${MODULE_NAME} MODULE_NAME_UPPER)
    set(MODULE_CONFIG_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${MODULE_NAME}/configs")

    # Pass source paths to configs for tests. This is module-global to allow e.g
    # scpimodule running tests with other modules
    add_compile_definitions(CONFIG_SOURCES_${MODULE_NAME_UPPER}="${MODULE_CONFIG_PATH}")

    # xml-schema resources are required by modules and tests. If we put resource into
    # our library both would need some call of Q_INIT_RESOURCE. So publish resource
    # location and compile in where needed.
    set(XML_SCHEMA_RESOURCE_${MODULE_NAME_UPPER} ${MODULE_CONFIG_PATH}/xml-schema.qrc)
endmacro()

# Notes
# * SET_MODULE_ENV !MUST! be set before add_subdirectory
# * Modules not added here don't support common module folder layout (yet)
SET_MODULE_ENV(adjustmentmodule)
SET_MODULE_ENV(dftmodule)
SET_MODULE_ENV(fftmodule)
SET_MODULE_ENV(lambdamodule)
SET_MODULE_ENV(oscimodule)
SET_MODULE_ENV(power1module)
SET_MODULE_ENV(power2module)
SET_MODULE_ENV(rangemodule)
SET_MODULE_ENV(referencemodule)
SET_MODULE_ENV(rmsmodule)
SET_MODULE_ENV(samplemodule)
SET_MODULE_ENV(scpimodule)
SET_MODULE_ENV(sec1module)
SET_MODULE_ENV(sem1module)
SET_MODULE_ENV(statusmodule)
SET_MODULE_ENV(thdnmodule)

add_subdirectory(adjustmentmodule)
add_subdirectory(burden1module)
add_subdirectory(dftmodule)
add_subdirectory(efficiency1module)
add_subdirectory(electronicburdenmodule)
add_subdirectory(fftmodule)
add_subdirectory(lambdamodule)
add_subdirectory(modemodule)
add_subdirectory(oscimodule)
add_subdirectory(power1module)
add_subdirectory(power2module)
add_subdirectory(power3module)
add_subdirectory(rangemodule)
add_subdirectory(referencemodule)
add_subdirectory(rmsmodule)
add_subdirectory(samplemodule)
add_subdirectory(scpimodule)
add_subdirectory(sec1module)
add_subdirectory(sem1module)
add_subdirectory(sourcemodule)
add_subdirectory(spm1module)
add_subdirectory(statusmodule)
add_subdirectory(thdnmodule)
add_subdirectory(transformer1module)
