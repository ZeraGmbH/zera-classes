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
    const VeinScpiModuleInterfaceParser::ScpiParseInfo allParamInfos = moduleInterfaces.getParamInfo();

    for (auto iter = allParamInfos.constBegin(); iter != allParamInfos.constEnd(); ++iter) {
        const QList<cSCPICmdInfoPtr> &params = iter.value();
        for (const cSCPICmdInfoPtr &param : params)
            addSCPICommand(scpiInterface, param);
    }
    return true;
}

void ScpiModelParameters::addSCPICommand(cSCPIInterface *scpiInterface, const cSCPICmdInfoPtr &scpiCmdInfo)
{
    const int entityId = scpiCmdInfo->entityId;
    const QString &componentName = scpiCmdInfo->componentOrRpcName;

    QStringList scpiFullPathList = scpiCmdInfo->scpiFullPathList();
    QString cmdNode = scpiFullPathList.takeLast();
    QString cmdParent = scpiFullPathList.join(':');

    ScpiBaseDelegatePtr delegate = std::make_shared<ScpiDelegateParameter>(ScpiDelegateParameter::Params{cmdParent, cmdNode, scpiCmdInfo->scpiCmdQueryFlags, entityId, componentName, m_scpiModule});
    ScpiDelegateXmlExportGenerator::setXmlComponentInfo(delegate, scpiCmdInfo->veinComponentInfo);
    scpiInterface->addSCPICommand(delegate);
}

}
