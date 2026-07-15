#include "scpimodelcatalogs.h"
#include "scpidelegatecatalog.h"
#include <QJsonDocument>
#include <QJsonArray>

namespace SCPIMODULE {

ScpiModelCatalogs::ScpiModelCatalogs(cSCPIModule *scpiModule) :
    m_scpiModule(scpiModule)
{
}

bool ScpiModelCatalogs::setupScpi(cSCPIInterface *scpiInterface)
{
    const VeinScpiModuleInterfaceParser moduleInterfaces = m_scpiModule->getScpiModuleInterfaceParser();
    const VeinScpiModuleInterfaceParser::ScpiParseInfo allCatalogInfos = moduleInterfaces.getCatalogInfo();

    for (auto iter = allCatalogInfos.constBegin(); iter != allCatalogInfos.constEnd(); ++iter) {
        const QList<cSCPICmdInfoPtr> &catalogs = iter.value();
        for (const cSCPICmdInfoPtr &command : catalogs)
            addSCPICommand(scpiInterface, command);
    }
    return true;
}

void ScpiModelCatalogs::actualizeCatalogs(const QVariant &modInterface)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(modInterface.toByteArray());
    if ( !jsonDoc.isNull() && jsonDoc.isObject() ) {
        QJsonObject jsonObj = jsonDoc.object();
        jsonObj = jsonObj["SCPIInfo"].toObject();
        QString scpiModuleName = jsonObj["Name"].toString();
        QJsonArray jsonArr = jsonObj["Cmd"].toArray();
        // we iterate over all cmds
        for (int j = 0; j < jsonArr.count(); j++) {
            QJsonArray jsonCmdArr = jsonArr[j].toArray();
            if (jsonCmdArr[4].toString() != "0") { // so it is a catalog delegate
                QString scpiPath = QString("%1:%2:%3").arg(jsonCmdArr[0].toString(), scpiModuleName, jsonCmdArr[1].toString());
                m_scpiCatalogDelegateHash[scpiPath]->setOutputFromInfModuleInterface(modInterface);
            }
        }
    }
}

void ScpiModelCatalogs::addSCPICommand(cSCPIInterface *scpiInterface, const cSCPICmdInfoPtr &scpiCmdInfo)
{
    QStringList scpiFullPathList = scpiCmdInfo->scpiFullPathList();
    QString cmdNode = scpiFullPathList.takeLast();
    QString cmdParent = scpiFullPathList.join(':');
    ScpiBaseDelegatePtr delegate = std::make_shared<ScpiDelegateCatalog>(ScpiDelegateCatalog::Params{cmdParent, cmdNode, scpiCmdInfo->scpiCmdQueryFlags, m_scpiModule, scpiCmdInfo});
    m_scpiCatalogDelegateHash[scpiCmdInfo->scpiFullPath()] = static_cast<ScpiDelegateCatalog*>(delegate.get()); // for easier access if we need to change answers of this delegate
    scpiInterface->addSCPICommand(delegate);
}

}
