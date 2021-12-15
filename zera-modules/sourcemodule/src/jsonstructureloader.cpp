#include "jsonstructureloader.h"
#include "jsonfilenames.h"
#include "sourcejsonapi.h"
#include <zera-jsonfileloader.h>
#include <zera-json-params-structure.h>

QJsonObject JsonStructureLoader::loadJsonDefaultStructure(SupportedSourceTypes type)
{
    return loadJsonStructureFromFile(JsonFilenames::getJsonStructurePath(type));
}

QJsonObject JsonStructureLoader::loadJsonStructure(SupportedSourceTypes type, QString deviceName)
{
    SourceJsonStructApi structureApi = SourceJsonStructApi(loadJsonDefaultStructure(type));
    if(!deviceName.isEmpty()) {
        structureApi.setDeviceName(deviceName);
    }
    return structureApi.getParamStructure();
}

QJsonObject JsonStructureLoader::loadJsonStructureFromFile(QString fileName)
{
    QJsonObject jsonStructureRaw = cJsonFileLoader::loadJsonFile(fileName);
    cZeraJsonParamsStructure jsonParamsStructure;
    jsonParamsStructure.loadStructure(jsonStructureRaw);
    return jsonParamsStructure.jsonStructure();
}

