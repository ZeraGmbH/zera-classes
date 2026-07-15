#include "veinscpimoduleinterfaceparser.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace SCPIMODULE
{

bool VeinScpiModuleInterfaceParser::parseVeinStorage(const VeinStorage::AbstractDatabase *storageDb)
{
    m_entitiesWithScpi.clear();
    m_componentInfo.clear();
    m_rpcInfo.clear();

    bool ok = true;
    const QList<int> entityIdList = storageDb->getEntityList();
    for(auto entityID : entityIdList) {
        // we parse over all moduleinterface components
        if (storageDb->hasStoredValue(entityID, QString("INF_ModuleInterface"))) {
            QJsonDocument jsonDoc = QJsonDocument::fromJson(storageDb->getStoredValue(entityID, QString("INF_ModuleInterface")).toByteArray());
            if ( !jsonDoc.isNull() && jsonDoc.isObject() ) {
                const QJsonObject jsonObj = jsonDoc.object();
                const QJsonObject jsonScpiInfo = jsonObj["SCPIInfo"].toObject();
                const QJsonObject jsonComponentInfo = jsonObj["ComponentInfo"].toObject();
                const QString scpiModuleName = jsonScpiInfo["Name"].toString();
                m_entitiesWithScpi[scpiModuleName] = entityID;

                QJsonArray jsonScpiCmdArr = jsonScpiInfo["Cmd"].toArray();
                for (int j = 0; j < jsonScpiCmdArr.count(); j++) {
                    cSCPICmdInfoPtr scpiCmdInfo = std::make_shared<cSCPICmdInfo>();
                    scpiCmdInfo->scpiModuleName = scpiModuleName;
                    scpiCmdInfo->entityId = entityID;
                    QJsonArray jsonCmdArr = jsonScpiCmdArr[j].toArray();
                    scpiCmdInfo->scpiModel = jsonCmdArr[0].toString();
                    scpiCmdInfo->scpiCommand = jsonCmdArr[1].toString();
                    scpiCmdInfo->scpiCmdQueryFlags = jsonCmdArr[2].toString().toUShort();
                    scpiCmdInfo->componentOrRpcName = jsonCmdArr[3].toString();
                    scpiCmdInfo->veinComponentInfo = jsonComponentInfo[scpiCmdInfo->componentOrRpcName].toObject();
                    scpiCmdInfo->refType = jsonCmdArr[4].toString();
                    if (scpiCmdInfo->scpiModel != "MEASURE" && scpiCmdInfo->refType == "0")
                        m_paramInfo[entityID].append(scpiCmdInfo);
                    else
                        m_componentInfo[entityID].append(scpiCmdInfo);
                }

                const QJsonObject jsonRpcInfo = jsonObj["RpcInfo"].toObject();
                QJsonArray jsonRpcScpiCmdArr = jsonScpiInfo["RPC"].toArray();
                for (int j = 0; j < jsonRpcScpiCmdArr.count(); j++) {
                    cSCPICmdInfoPtr scpiCmdInfo = std::make_shared<cSCPICmdInfo>();
                    scpiCmdInfo->scpiModuleName = scpiModuleName;
                    scpiCmdInfo->entityId = entityID;
                    QJsonArray jsonCmdArr = jsonRpcScpiCmdArr[j].toArray();
                    scpiCmdInfo->scpiModel = jsonCmdArr[0].toString();
                    scpiCmdInfo->scpiCommand = jsonCmdArr[1].toString();
                    scpiCmdInfo->scpiCmdQueryFlags = jsonCmdArr[2].toString().toUShort();
                    scpiCmdInfo->componentOrRpcName = jsonCmdArr[3].toString();
                    scpiCmdInfo->veinComponentInfo = jsonRpcInfo[scpiCmdInfo->componentOrRpcName].toObject();
                    scpiCmdInfo->refType = jsonCmdArr[4].toString();

                    m_rpcInfo[entityID].append(scpiCmdInfo);
                }
            }
            else
                ok = false;
        }
    }
    return ok;
}

const VeinScpiModuleInterfaceParser::ScpiParseInfo &VeinScpiModuleInterfaceParser::getParamInfo() const
{
    return m_paramInfo;
}

const VeinScpiModuleInterfaceParser::ScpiEntityHash &VeinScpiModuleInterfaceParser::getEntitiesWithScpi() const
{
    return m_entitiesWithScpi;
}

const VeinScpiModuleInterfaceParser::ScpiParseInfo &VeinScpiModuleInterfaceParser::getComponentInfo() const
{
    return m_componentInfo;
}

const VeinScpiModuleInterfaceParser::ScpiParseInfo &VeinScpiModuleInterfaceParser::getRpcInfo() const
{
    return m_rpcInfo;
}

}
