#include "scpidelegatecatalog.h"
#include <zscpi_response_definitions.h>
#include <QJsonArray>
#include <QVariantList>

namespace SCPIMODULE {

ScpiDelegateCatalog::ScpiDelegateCatalog(const Params &params) :
    ScpiDelegateTemplate(params.cmdParent, params.cmd, params.scpiQueryCmdFlags),
    m_pModule(params.scpimodule),
    m_pSCPICmdInfo(params.scpicmdinfo)
{
    setOutputInitial(m_pSCPICmdInfo);
}

void ScpiDelegateCatalog::executeSCPI(cSCPIClient *client, const QString &scpi, const ScpiTransactionId &scpiTransactionId)
{
    quint8 scpiQueryCmdFlags = getType();
    cSCPICommand cmd = scpi;
    if (cmd.isQuery() && ((scpiQueryCmdFlags & SCPI::isQuery) > 0))// test if we got an allowed query
        client->handleCmdFinish(m_sAnswer, scpiTransactionId);
    else
        client->handleCmdFinishStatusOnly(ZSCPI::nak, scpiTransactionId);
}

void ScpiDelegateCatalog::setOutputFromInfModuleInterface(const QJsonObject &modInterfaceJson)
{
    const QJsonObject componentInfos = modInterfaceJson["ComponentInfo"].toObject();
    const QJsonObject componentInfo = componentInfos[m_pSCPICmdInfo->componentOrRpcName].toObject();
    setOutputFromComponentInfo(componentInfo);
}

void ScpiDelegateCatalog::setOutputInitial(const cSCPICmdInfoPtr &scpicmdinfo)
{
    setOutputFromComponentInfo(scpicmdinfo->veinComponentInfo);
}

void ScpiDelegateCatalog::setOutputFromComponentInfo(const QJsonObject &componentInfo)
{
    const QJsonObject validation = componentInfo["Validation"].toObject();
    const QJsonArray validatorEntries = validation["Data"].toArray();

    const QVariantList validatorEntriesVariant = validatorEntries.toVariantList();
    m_sAnswer.clear();
    for(int i=0; i<validatorEntriesVariant.count(); ++i) {
        if(i>0)
            m_sAnswer += ";";
        m_sAnswer += validatorEntriesVariant.at(i).toString();
    }
}

}
