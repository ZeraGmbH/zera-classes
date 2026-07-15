#include "scpimodelparameters.h"
#include "scpidelegateparameter.h"
#include "scpidelegatexmlexportgenerator.h"

namespace SCPIMODULE {

ScpiModelParameters::ScpiModelParameters(cSCPIModule* scpiModule) :
    m_scpiModule(scpiModule)
{
}

bool ScpiModelParameters::setupScpi(cSCPIInterface *scpiInterface)
{
    const VeinScpiModuleInterfaceParser moduleInterfaces = m_scpiModule->getScpiModuleInterfaceParser();
    const VeinScpiModuleInterfaceParser::ScpiParseInfo allComponentInfos = moduleInterfaces.getComponentInfo();
    const VeinStorage::AbstractDatabase* storageDb = m_scpiModule->getStorageDb();

    const QList<int> entityIdList = storageDb->getEntityList();
    for(auto entityID : entityIdList) {
        if (allComponentInfos.contains(entityID)) {
            const QList<cSCPICmdInfoPtr> &commands = allComponentInfos[entityID];
            for (const cSCPICmdInfoPtr &command : commands)
                addSCPICommand(scpiInterface, command);
        }
    }
    return true;
}

void ScpiModelParameters::addSCPICommand(cSCPIInterface *scpiInterface, const cSCPICmdInfoPtr &scpiCmdInfo)
{
    const int entityId = scpiCmdInfo->entityId;
    const QString &componentName = scpiCmdInfo->componentOrRpcName;
    if (scpiCmdInfo->scpiModel != "MEASURE") {
        QString scpiPath = QString("%1:%2:%3").arg(scpiCmdInfo->scpiModel, scpiCmdInfo->scpiModuleName, scpiCmdInfo->scpiCommand);
        QStringList nodeNames = scpiPath.split(':');
        QString cmdNode = nodeNames.takeLast();
        QString cmdParent = nodeNames.join(':');
        ScpiBaseDelegatePtr delegate;
        if (scpiCmdInfo->refType == "0") {
            delegate = std::make_shared<ScpiDelegateParameter>(ScpiDelegateParameter::Params{cmdParent, cmdNode, scpiCmdInfo->scpiCmdQueryFlags, entityId, componentName, m_scpiModule});
            ScpiDelegateXmlExportGenerator::setXmlComponentInfo(delegate, scpiCmdInfo->veinComponentInfo);
            scpiInterface->addSCPICommand(delegate);
        }
    }
}

}
