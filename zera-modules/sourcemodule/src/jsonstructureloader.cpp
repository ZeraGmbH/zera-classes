#include "jsonstructureloader.h"
#include "jsonfilenames.h"
#include "sourcejsonapi.h"
#include <zera-jsonfileloader.h>
#include <zera-json-params-structure.h>

QJsonObject JsonStructureLoader::loadJsonDefaultStructure(SupportedSourceTypes type)
{
    return loadJsonStructureFromFile(JsonFilenames::getJsonStructurePath(type));
}

QJsonObject JsonStructureLoader::loadJsonStructure(SupportedSourceTypes type, QString deviceName, QString deviceVersion)
{
    SourceJsonStructApi structureApi = SourceJsonStructApi(loadJsonDefaultStructure(type));
    if(!deviceName.isEmpty()) {
        structureApi.setDeviceName(deviceName);
    }
    if(!deviceVersion.isEmpty()) {
        structureApi.setDeviceVersion(deviceVersion);
    }
    return structureApi.getParamStructure();
}

QJsonObject JsonStructureLoader::loadJsonStructureFromFile(QString fileName)
{
    QJsonObject jsonStructureRaw = cJsonFileLoader::loadJsonFile(fileName);
    ZeraJsonParamsStructure jsonParamsStructure;
    jsonParamsStructure.setJson(jsonStructureRaw);
    return jsonParamsStructure.getJson();
}

