# Pass source paths to configs for tests. This is module-global to allow e.g
# scpimodule running tests with other modules
# Note: These definitions MUST be set before adding subdirectories
add_definitions(-DCONFIG_SOURCES_ADJUSTMENTMODULE="${CMAKE_CURRENT_SOURCE_DIR}/adjustmentmodule/configs")
add_definitions(-DCONFIG_SOURCES_POWER1MODULE="${CMAKE_CURRENT_SOURCE_DIR}/power1module/configs")
add_definitions(-DCONFIG_SOURCES_POWER2MODULE="${CMAKE_CURRENT_SOURCE_DIR}/power2module/configs")
add_definitions(-DCONFIG_SOURCES_RANGEMODULE="${CMAKE_CURRENT_SOURCE_DIR}/rangemodule/configs")
add_definitions(-DCONFIG_SOURCES_SCPIMODULE="${CMAKE_CURRENT_SOURCE_DIR}/scpimodule/configs")
add_definitions(-DCONFIG_SOURCES_SEC1MODULE="${CMAKE_CURRENT_SOURCE_DIR}/sec1module/configs")
add_definitions(-DCONFIG_SOURCES_STATUSMODULE="${CMAKE_CURRENT_SOURCE_DIR}/statusmodule/configs")

# xml-schema resource is required by modules and tests. If we put resource into
# our library both would need some call of Q_INIT_RESOURCE. So publish resource
# location and compile in where needed.
# Note: These definitions MUST be set before adding subdirectories
set(XML_SCHEMA_RESOURCE_ADJUSTMENTMODULE ${CMAKE_CURRENT_SOURCE_DIR}/adjustmentmodule/configs/xml-schema.qrc)
set(XML_SCHEMA_RESOURCE_POWER1MODULE ${CMAKE_CURRENT_SOURCE_DIR}/power1module/configs/xml-schema.qrc)
set(XML_SCHEMA_RESOURCE_POWER2MODULE ${CMAKE_CURRENT_SOURCE_DIR}/power2module/configs/xml-schema.qrc)
set(XML_SCHEMA_RESOURCE_RANGEMODULE ${CMAKE_CURRENT_SOURCE_DIR}/rangemodule/configs/xml-schema.qrc)
set(XML_SCHEMA_RESOURCE_SCPIMODULE ${CMAKE_CURRENT_SOURCE_DIR}/scpimodule/configs/xml-schema.qrc)
set(XML_SCHEMA_RESOURCE_SEC1MODULE ${CMAKE_CURRENT_SOURCE_DIR}/sec1module/configs/xml-schema.qrc)
set(XML_SCHEMA_RESOURCE_STATUSMODULE ${CMAKE_CURRENT_SOURCE_DIR}/statusmodule/configs/xml-schema.qrc)

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
