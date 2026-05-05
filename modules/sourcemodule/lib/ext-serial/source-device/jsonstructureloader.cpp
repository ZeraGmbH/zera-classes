#include "jsonstructureloader.h"
#include "jsonfilenames.h"
#include "jsonstructapi.h"
#include <zera-jsonfileloader.h>
#include <zera-json-params-structure.h>

QJsonObject JsonStructureLoader::loadJsonDefaultStructure(const SupportedSourceTypes &type)
{
    return loadJsonStructureFromFile(JsonFilenames::getJsonStructurePath(type));
}

QJsonObject JsonStructureLoader::loadJsonStructure(const SupportedSourceTypes &type, const QString &deviceName, const QString &deviceVersion)
{
    JsonStructApi structureApi = JsonStructApi(loadJsonDefaultStructure(type));
    if(!deviceName.isEmpty()) {
        structureApi.setDeviceName(deviceName);
    }
    if(!deviceVersion.isEmpty()) {
        structureApi.setDeviceVersion(deviceVersion);
    }
    return structureApi.getParamStructure();
}

QJsonObject JsonStructureLoader::loadJsonStructure(const SourceProperties &properties)
{
    return loadJsonStructure(properties.getType(), properties.getName(), properties.getVersion());
}

QJsonObject JsonStructureLoader::loadJsonStructureFromFile(QString fileName)
{
    QJsonObject jsonStructureRaw = cJsonFileLoader::loadJsonFile(fileName);
    ZeraJsonParamsStructure jsonParamsStructure;
    jsonParamsStructure.setJson(jsonStructureRaw);
    return jsonParamsStructure.getJson();
}

