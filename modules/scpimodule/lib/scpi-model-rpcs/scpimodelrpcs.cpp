#include "scpimodelrpcs.h"
#include "scpidelegaterpc.h"
#include "scpidelegatexmlexportgenerator.h"

namespace SCPIMODULE {

ScpiModelRpcs::ScpiModelRpcs(cSCPIModule* scpiModule) :
    m_scpiModule(scpiModule)
{
}

bool ScpiModelRpcs::setupScpi(cSCPIInterface *scpiInterface)
{
    const VeinScpiModuleInterfaceParser moduleInterfaces = m_scpiModule->getScpiModuleInterfaceParser();
    const VeinScpiModuleInterfaceParser::ScpiParseInfo allRpcInfos = moduleInterfaces.getRpcInfo();

    for (auto iter = allRpcInfos.constBegin(); iter != allRpcInfos.constEnd(); ++iter) {
        const QList<cSCPICmdInfoPtr> &rpcs = iter.value();
        for (const cSCPICmdInfoPtr &rpc : rpcs)
            addRPCCommand(scpiInterface, rpc);
    }

    return true;
}

void ScpiModelRpcs::addRPCCommand(cSCPIInterface *scpiInterface, const cSCPICmdInfoPtr &scpiCmdInfo)
{
    QString cmdComplete = QString("%1:%2:%3").arg(scpiCmdInfo->scpiModel, scpiCmdInfo->scpiModuleName, scpiCmdInfo->scpiCommand);
    QStringList nodeNames = cmdComplete.split(':');
    QString cmdNode = nodeNames.takeLast();
    QString cmdParent = nodeNames.join(':');
    ScpiBaseDelegatePtr delegate = std::make_shared<ScpiDelegateRpc>(ScpiDelegateRpc::Params{cmdParent, cmdNode, scpiCmdInfo->scpiCmdQueryFlags, scpiCmdInfo->entityId, scpiCmdInfo->componentOrRpcName, scpiCmdInfo->veinComponentInfo, m_scpiModule});
    ScpiDelegateXmlExportGenerator::setXmlComponentInfo(delegate, scpiCmdInfo->veinComponentInfo);
    scpiInterface->addSCPICommand(delegate);
}

}
