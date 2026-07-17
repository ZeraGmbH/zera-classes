#include "scpimodelrpcs.h"
#include "scpidelegaterpc.h"
#include "scpidelegatexmlexportgenerator.h"

namespace SCPIMODULE {

ScpiModelRpcs::ScpiModelRpcs(cSCPIModule* scpiModule) :
    m_scpiModule(scpiModule)
{
}

void ScpiModelRpcs::setupScpi(cSCPIInterface *scpiInterface)
{
    const ScpiVeinParser moduleInterfaces = m_scpiModule->getScpiModuleInterfaceParser();
    const ScpiVeinParser::ScpiParseInfo allRpcInfos = moduleInterfaces.getRpcInfo();

    for (auto iter = allRpcInfos.constBegin(); iter != allRpcInfos.constEnd(); ++iter) {
        const ScpiVeinParser::ScpiComponentParseInfo &rpcs = iter.value();
        for (const cSCPICmdInfoPtr &rpc : rpcs)
            addRPCCommand(scpiInterface, rpc);
    }
}

void ScpiModelRpcs::addRPCCommand(cSCPIInterface *scpiInterface, const cSCPICmdInfoPtr &scpiCmdInfo)
{
    QStringList scpiFullPathList = scpiCmdInfo->scpiFullPathList();
    QString cmdNode = scpiFullPathList.takeLast();
    QString cmdParent = scpiFullPathList.join(':');
    ScpiBaseDelegatePtr delegate = std::make_shared<ScpiDelegateRpc>(ScpiDelegateRpc::Params{cmdParent, cmdNode, scpiCmdInfo->scpiQueryCmdFlags, scpiCmdInfo->entityId, scpiCmdInfo->componentOrRpcName, scpiCmdInfo->veinComponentInfo, m_scpiModule});
    ScpiDelegateXmlExportGenerator::setXmlComponentInfo(delegate, scpiCmdInfo->veinComponentInfo);
    scpiInterface->addSCPICommand(delegate);
}

}
