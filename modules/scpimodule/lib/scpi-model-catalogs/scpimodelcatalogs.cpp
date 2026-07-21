#include "scpimodelcatalogs.h"
#include "scpidelegatecatalog.h"
#include <QJsonDocument>
#include <QJsonArray>

namespace SCPIMODULE {

ScpiModelCatalogs::ScpiModelCatalogs(cSCPIModule *scpiModule) :
    m_scpiModule(scpiModule)
{
}

void ScpiModelCatalogs::setupScpi(cSCPIInterface *scpiInterface)
{
    const ScpiVeinParser moduleInterfaces = m_scpiModule->getScpiModuleInterfaceParser();
    const ScpiVeinParser::ScpiParseInfo allCatalogInfos = moduleInterfaces.getCatalogInfo();

    for (auto iter = allCatalogInfos.constBegin(); iter != allCatalogInfos.constEnd(); ++iter) {
        const ScpiVeinParser::ScpiComponentParseInfo &catalogs = iter.value();
        for (const cSCPICmdInfoPtr &command : catalogs)
            addSCPICommand(scpiInterface, command);
    }
}

void ScpiModelCatalogs::actualizeCatalogs(const QVariant &modInterface)
{
    QJsonDocument modInterfaceJsonDoc = QJsonDocument::fromJson(modInterface.toByteArray());
    if ( !modInterfaceJsonDoc.isNull() && modInterfaceJsonDoc.isObject() ) {
        const QJsonObject modInterfaceJson = modInterfaceJsonDoc.object();
        const QJsonObject scpiInfo = modInterfaceJson["SCPIInfo"].toObject();
        const QString scpiModuleName = scpiInfo["Name"].toString();
        const QJsonArray cmdArray = scpiInfo["Cmd"].toArray();
        // we iterate over all cmds
        for (int j = 0; j < cmdArray.count(); j++) {
            const QJsonArray cmdEntries = cmdArray[j].toArray();
            if (cmdEntries[4].toString() != "0") { // so it is a catalog delegate
                const QString scpiPath = QString("%1:%2:%3").arg(cmdEntries[0].toString(), scpiModuleName, cmdEntries[1].toString());
                m_scpiCatalogDelegateHash[scpiPath]->setOutputFromInfModuleInterface(modInterfaceJson);
            }
        }
    }
}

void ScpiModelCatalogs::addSCPICommand(cSCPIInterface *scpiInterface, const cSCPICmdInfoPtr &scpiCmdInfo)
{
    QStringList scpiFullPathList = scpiCmdInfo->scpiFullPathList();
    QString cmdNode = scpiFullPathList.takeLast();
    QString cmdParent = scpiFullPathList.join(':');
    ScpiBaseDelegatePtr delegate = std::make_shared<ScpiDelegateCatalog>(ScpiDelegateCatalog::Params{cmdParent, cmdNode, scpiCmdInfo->scpiQueryCmdFlags, m_scpiModule, scpiCmdInfo});
    m_scpiCatalogDelegateHash[scpiCmdInfo->scpiFullPath()] = static_cast<ScpiDelegateCatalog*>(delegate.get()); // for easier access if we need to change answers of this delegate
    scpiInterface->addSCPICommand(delegate);
}

}
